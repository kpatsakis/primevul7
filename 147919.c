mj_print_page(gx_device_printer * pdev, gp_file * prn_stream, int ptype)
{
/*  int line_size = gdev_prn_rasterwidth(pdev, 0); */
  int line_size = gdev_prn_raster(pdev);
  int line_size_words = (line_size + W - 1) / W;
  int num_comps = pdev->color_info.num_components;
  int bits_per_pixel = pdev->color_info.depth;
  int storage_bpp = bits_per_pixel;
  int expanded_bpp = bits_per_pixel;
  int plane_size, databuff_size;
  int errbuff_size = 0;
  int outbuff_size = 0;
  int scan = 0;
  int *errors[2];
  byte *data[4], *plane_data[4][4], *out_data;
  byte *out_row;
  word *storage;
  uint storage_size_words;
  uint mj_tmp_buf_size;
  byte* mj_tmp_buf;
  int xtalbuff_size;
  short *xtalbuff;
  short *Cbar[16];
  short *Mbar[16];
  short *Ybar[16];
  short *Kbar[16];
  short *Cbuf[2];
  short *Mbuf[2];
  short *Ybuf[2];
  short *Kbuf[2];

  /* Tricks and cheats ... */
  if (num_comps == 3) num_comps = 4;            /* print CMYK */

  if (storage_bpp == 8 && num_comps >= 3)
    bits_per_pixel = expanded_bpp = 3;  /* Only 3 bits of each byte used */

  plane_size = calc_buffsize(line_size, storage_bpp);

  if (bits_per_pixel == 1) {            /* Data printed direct from i/p */
    databuff_size = 0;                  /* so no data buffer required, */
    outbuff_size = plane_size * 4;      /* but need separate output buffers */
  }

  if (bits_per_pixel > 4) {             /* Error buffer for FS dithering */
    expanded_bpp = storage_bpp =        /* 8, 24 or 32 bits */
      num_comps * 8;
    errbuff_size =                      /* 4n extra values for line ends */
      calc_buffsize((plane_size * expanded_bpp + num_comps * 4) * I, 1);
  }

  databuff_size = plane_size * storage_bpp;

  storage_size_words = ((plane_size + plane_size) * num_comps +
                        databuff_size + errbuff_size + outbuff_size) / W;

/* NOZ */
  xtalbuff_size = plane_size*8 + 64;
  xtalbuff = (short *) gs_malloc(pdev->memory->non_gc_memory,  xtalbuff_size*(16*4+2*4) , W, "mj_colour_print_barrier");
  memset(xtalbuff, 0, xtalbuff_size*(16*4+2*4) * W);
  {
        int i;
        short *p = xtalbuff + 16;
        for ( i = 0 ; i < 16 ; i++ ) {
                Cbar[i] = p;
                p += xtalbuff_size;
        }
        for ( i = 0 ; i < 16 ; i++ ) {
                Mbar[i] = p;
                p += xtalbuff_size;
        }
        for ( i = 0 ; i < 16 ; i++ ) {
                Ybar[i] = p;
                p += xtalbuff_size;
        }
        for ( i = 0 ; i < 16 ; i++ ) {
                Kbar[i] = p;
                p += xtalbuff_size;
        }
        Cbuf[0] = p;
        p += xtalbuff_size;
        Cbuf[1] = p;
        p += xtalbuff_size;
        Mbuf[0] = p;
        p += xtalbuff_size;
        Mbuf[1] = p;
        p += xtalbuff_size;
        Ybuf[0] = p;
        p += xtalbuff_size;
        Ybuf[1] = p;
        p += xtalbuff_size;
        Kbuf[0] = p;
        p += xtalbuff_size;
        Kbuf[1] = p;
        p += xtalbuff_size;
  }

  storage = (word *) gs_malloc(pdev->memory->non_gc_memory, storage_size_words, W, "mj_colour_print_page");

/* prepare a temporary buffer for mj_raster_cmd */

  mj_tmp_buf_size = plane_size;
  mj_tmp_buf = (byte *) gs_malloc(pdev->memory->non_gc_memory, mj_tmp_buf_size, W ,"mj_raster_buffer");

#if 0
  dprintf1("storage_size_words :%d\n", storage_size_words);
  dprintf1("mj_tmp_buf_size :%d\n", mj_tmp_buf_size);
#endif
  /*
   * The principal data pointers are stored as pairs of values, with
   * the selection being made by the 'scan' variable. The function of the
   * scan variable is overloaded, as it controls both the alternating
   * raster scan direction used in the Floyd-Steinberg dithering and also
   * the buffer alternation required for line-difference compression.
   *
   * Thus, the number of pointers required is as follows:
   *
   *   errors:      2  (scan direction only)
   *   data:        4  (scan direction and alternating buffers)
   *   plane_data:  4  (scan direction and alternating buffers)
   */

  if (storage == NULL || mj_tmp_buf == NULL) /* can't allocate working area */
    return_error(gs_error_VMerror);
  else {
    int i;
    byte *p = out_data = out_row = (byte *)storage;
    data[0] = data[1] = data[2] = p;
    data[3] = p + databuff_size;
    if (bits_per_pixel > 1) {
      p += databuff_size;
    }
    if (bits_per_pixel > 4) {
      errors[0] = (int *)p + num_comps * 2;
      errors[1] = errors[0] + databuff_size;
      p += errbuff_size;
    }
    for (i = 0; i < num_comps; i++) {
      plane_data[0][i] = plane_data[2][i] = p;
      p += plane_size;
    }
    for (i = 0; i < num_comps; i++) {
      plane_data[1][i] = p;
      plane_data[3][i] = p + plane_size;
      p += plane_size;
    }
    if (bits_per_pixel == 1) {
      out_data = out_row = p;	  /* size is outbuff_size * 4 */
      data[1] += databuff_size;   /* coincides with plane_data pointers */
      data[3] += databuff_size;
    }
  }

  /* Clear temp storage */
  memset(storage, 0, storage_size_words * W);

  /* Initialize printer. */
  {
    /** Reset printer, enter graphics mode: */

    gp_fwrite("\033@\033(G\001\000\001", sizeof(byte), 8, prn_stream);

    /** Micro-weave-Mode */
    if (mj->microweave) {
      gp_fwrite("\033(i\001\000\001", sizeof(byte), 6, prn_stream);
    }
    /** Dot-Size define */
    if (mj->dotsize) {
      gp_fwrite("\033(e\002\000\000\001", sizeof(byte), 7, prn_stream);
    }

    if (ptype == MJ6000C || ptype == MJ8000C) {
      /* Select Monochrome/Color Printing Mode Command */
      if (pdev->color_info.depth == 8)
        gp_fwrite("\033(K\002\000\000\001", sizeof(byte), 7, prn_stream);
    }

    if (mj->direction) /* set the direction of the head */
      gp_fwrite("\033U\1", 1, 3, prn_stream); /* Unidirectional Printing */
    else
      gp_fwrite("\033U\0", 1, 3, prn_stream);

#if 0
#ifdef A4
        /*
        ** After reset, the Stylus is set up for US letter paper.
        ** We need to set the page size appropriately for A4 paper.
        ** For some bizarre reason the ESC/P2 language wants the bottom
        ** margin measured from the *top* of the page:
        */

        gp_fwrite("\033(U\001\0\n\033(C\002\0t\020\033(c\004\0\0\0t\020",
                                                        1, 22, prn_stream);
#endif
#endif

        /*
        ** Set the line spacing to match the band height:
        */

        if( pdev->y_pixels_per_inch >= 720 )
          gp_fwrite("\033(U\001\0\005\033+\001", sizeof(byte), 9, prn_stream);
        else if( pdev->y_pixels_per_inch >= 360 )
           gp_fwrite("\033(U\001\0\012\033+\001", sizeof(byte), 9, prn_stream);
        else /* 180 dpi */
           gp_fwrite("\033(U\001\0\024\033+\002", sizeof(byte), 9, prn_stream);

    /* set the length of the page */
        gp_fwrite("\033(C\2\0", sizeof(byte), 5, prn_stream);
        gp_fputc(((pdev->height) % 256), prn_stream);
        gp_fputc(((pdev->height) / 256), prn_stream);
  }

#define MOFFSET (pdev->t_margin - MJ700V2C_PRINT_LIMIT) /* Print position */

  {
    int MJ_MARGIN_MM = 55;
    uint top_skip = ( MJ_MARGIN_MM  * pdev->y_pixels_per_inch ) / 254;
    top_skip = (top_skip ^ (-1)) & 65536;
    gp_fwrite("\033(V\2\0\0\0",sizeof(byte), 7, prn_stream);
    gp_fwrite("\033(v\2\0\0\xff",sizeof(byte), 7, prn_stream);
  }

  /* Send each scan line in turn */
  {
    long int lend = pdev->height -
      (dev_t_margin_points(pdev) + dev_b_margin_points(pdev));
    int cErr, mErr, yErr, kErr;
    int this_pass, i;
    long int lnum;
    int num_blank_lines = 0;
    int start_rows = (num_comps == 1) ?
      HEAD_ROWS_MONO - 1 : HEAD_ROWS_COLOUR - 1;
    word rmask = ~(word) 0 << ((-pdev->width * storage_bpp) & (W * 8 - 1));

    cErr = mErr = yErr = kErr = 0;

    if (bits_per_pixel > 4) { /* Randomly seed initial error buffer */
      int *ep = errors[0];
      for (i = 0; i < databuff_size; i++) {
        *ep++ = (rand() % (MAXVALUE / 2))  - MAXVALUE / 4;
      }
    }

    this_pass = start_rows;

    lnum = 0;

    /* for Debug */

    for (; lnum < lend; lnum++) {
      word *data_words = (word *)data[scan];
      register word *end_data = data_words + line_size_words;
      gx_color_index *p_data;

      gdev_prn_copy_scan_lines(pdev, lnum, data[scan], line_size);

      /* Mask off 1-bits beyond the line width. */
      end_data[-1] &= rmask;

      /* Remove trailing 0s. */
      while (end_data > data_words && end_data[-1] == 0)
        end_data--;
      if (end_data == data_words) {	/* Blank line */
        num_blank_lines++;
        continue; /* skip to  for (lnum) loop */
      }
      /* Skip blank lines if any */
      if (num_blank_lines > 0 ) {
        mj_v_skip(num_blank_lines, pdev, prn_stream);
        memset(plane_data[1 - scan][0], 0, plane_size * num_comps);
        num_blank_lines = 0;
        this_pass = start_rows;
      }

      /* Correct color depth. */
          if (mj->density != 1024 || mj->yellow != 1024 || mj->cyan != 1024
                  || mj->magenta != 1024 || mj->black != 1024 ) {
              for (p_data = (gx_color_index*) data_words; p_data < (gx_color_index *)end_data; p_data++) {
                        *p_data = mjc_correct_color(pdev, *p_data);
              }
          }

      {			/* Printing non-blank lines */
        register byte *kP = plane_data[scan + 2][3];
        register byte *cP = plane_data[scan + 2][2];
        register byte *mP = plane_data[scan + 2][1];
        register byte *yP = plane_data[scan + 2][0];
        register byte *dp = data[scan + 2];
        int i, j;
        byte *odp;

        if (this_pass)
          this_pass--;
        else
          this_pass = start_rows;

        if (expanded_bpp > bits_per_pixel)   /* Expand line if required */
          expand_line(data_words, line_size, bits_per_pixel, expanded_bpp);

        /* In colour modes, we have some bit-shuffling to do before
         * we can print the data; in FS mode we also have the
         * dithering to take care of. */
        switch (expanded_bpp) {    /* Can be 1, 3, 8, 24 or 32 */
        case 3:
          /* Transpose the data to get pixel planes. */
          for (i = 0, odp = plane_data[scan][0]; i < databuff_size;
               i += 8, odp++) {	/* The following is for 16-bit
                                 * machines */
#define spread3(c)\
    { 0, c, c*0x100, c*0x101, c*0x10000L, c*0x10001L, c*0x10100L, c*0x10101L }
            static word spr40[8] = spread3(0x40);
            static word spr08[8] = spread3(8);
            static word spr02[8] = spread3(2);
            register byte *dp = data[scan] + i;
            register word pword =
            (spr40[dp[0]] << 1) +
            (spr40[dp[1]]) +
            (spr40[dp[2]] >> 1) +
            (spr08[dp[3]] << 1) +
            (spr08[dp[4]]) +
            (spr08[dp[5]] >> 1) +
            (spr02[dp[6]]) +
            (spr02[dp[7]] >> 1);
            odp[0] = (byte) (pword >> 16);
            odp[plane_size] = (byte) (pword >> 8);
            odp[plane_size * 2] = (byte) (pword);
          }
          break;

        case 8:
          FSDline(scan, i, j, plane_size, cErr, mErr, yErr, kErr,
                  cP, mP, yP, kP, 1);
          break;
        case 24:
          FSDline(scan, i, j, plane_size, cErr, mErr, yErr, kErr,
                  cP, mP, yP, kP, 3);
          break;
        case 32:
                if (scan == 1) {
                        dp -= plane_size*8*4;
                        cP -= plane_size;
                        mP -= plane_size;
                        yP -= plane_size;
                        kP -= plane_size;
                }
/*
{
        byte *p = dp;
        int i;
        for ( i = 0 ; i < plane_size ; i++ ) {
                dprintf4 ( "[%02X%02X%02X%02X]" , p[0] , p[1] , p[2] , p[3] );
                p += 4;
        }
        dprintf("\n");

}
*/
/*
          FSDline(scan, i, j, plane_size, cErr, mErr, yErr, kErr,
                  cP, mP, yP, kP, 4);
*/
/* NOZ */
          xtal_plane( dp++ , Kbuf , kP , Kbar , plane_size , xtalbuff_size );
          xtal_plane( dp++ , Cbuf , cP , Cbar , plane_size , xtalbuff_size );
          xtal_plane( dp++ , Mbuf , mP , Mbar , plane_size , xtalbuff_size );
          xtal_plane( dp++ , Ybuf , yP , Ybar , plane_size , xtalbuff_size );

          break;

        } /* switch(expanded_bpp) */

        /* Make sure all black is in the k plane */
        if (num_comps == 4 ) {
          if (mj->colorcomp > 3 ) {
            register word *kp = (word *)plane_data[scan][3];
            register word *cp = (word *)plane_data[scan][2];
            register word *mp = (word *)plane_data[scan][1];
            register word *yp = (word *)plane_data[scan][0];
            if (bits_per_pixel > 4) {  /* This has been done as 4 planes */
#if 0
              for (i = 0; i < plane_size / W; i++) {
                word bits = ~*kp++;
                *cp++ &= bits;
                *mp++ &= bits;
                *yp++ &= bits;
              }
#endif
            } else {  /* This has really been done as 3 planes */
              for (i = 0; i < plane_size / W; i++) {
                word bits = *cp & *mp & *yp;
                *kp++ = bits;
                bits = ~bits;
                *cp++ &= bits;
                *mp++ &= bits;
                *yp++ &= bits;
              }
            }
          } else if (mj->colorcomp == 3 ) {
            register word *kp = (word *)plane_data[scan][3];
            register word *cp = (word *)plane_data[scan][2];
            register word *mp = (word *)plane_data[scan][1];
            register word *yp = (word *)plane_data[scan][0];
            if (bits_per_pixel > 4) {  /* This has been done as 4 planes */
              for (i = 0; i < plane_size / W; i++) {
                word bits = *kp++; /* kp will not be used when printing */
                *cp++ |= bits;
                *mp++ |= bits;
                *yp++ |= bits;
              }
            } else {  /* This has really been done as 3 planes */
            }
          }
        }

        /* Transfer raster graphics
         * in the order (K), C, M, Y. */
        switch (mj->colorcomp) {
        case 1:
          out_data = (byte*) plane_data[scan][0];
          /* 3 for balck */
          mj_raster_cmd(3, plane_size, out_data, mj_tmp_buf, pdev, prn_stream);
          break;
        case 3:
          for (i = 3 - 1; i >= 0; i--) {
            out_data = (byte*) plane_data[scan][i];
            mj_raster_cmd(i, plane_size, out_data, mj_tmp_buf, pdev, prn_stream);
          }
          break;
        default:
          for (i = num_comps - 1; i >= 0; i--) {
            out_data = (byte*) plane_data[scan][i];
            mj_raster_cmd(i, plane_size, out_data, mj_tmp_buf, pdev, prn_stream);
          }
          break;
        } /* Transfer Raster Graphics ... */

        {
          if ( pdev->y_pixels_per_inch > 360 ) {
             gp_fwrite("\033(v\2\0\1\0",sizeof(byte),7, prn_stream);
           } else {
             gp_fputc('\n', prn_stream);
           }
        }
        scan = 1 - scan;          /* toggle scan direction */
      }	  /* Printing non-blank lines */
    }     /* for lnum ... */
  }       /* send each scan line in turn */

  /* end raster graphics & reset printer */

  /* eject page */
  {
    gp_fputs("\f\033@", prn_stream);
    gp_fflush(prn_stream);
  }
  /* free temporary storage */
  gs_free(pdev->memory->non_gc_memory, (char *) storage, storage_size_words, W, "mj_colour_print_page");
  gs_free(pdev->memory->non_gc_memory, (char *) mj_tmp_buf, mj_tmp_buf_size, W, "mj_raster_buffer");
  gs_free(pdev->memory->non_gc_memory, (char *) xtalbuff , xtalbuff_size*(16*4+2*4) , W, "mj_colour_print_barrier");

  return 0;
}