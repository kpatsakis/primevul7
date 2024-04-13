ReadImage (FILE        *fd,
           const gchar *filename,
           gint         len,
           gint         height,
           CMap         cmap,
           gint         ncols,
           gint         format,
           gint         interlace,
           gint         number,
           guint        leftpos,
           guint        toppos,
           guint        screenwidth,
           guint        screenheight)
{
  static gint32 image_ID   = -1;
  static gint   frame_number = 1;

  gint32        layer_ID;
  GimpPixelRgn  pixel_rgn;
  GimpDrawable *drawable;
  guchar       *dest, *temp;
  guchar        c;
  gint          xpos = 0, ypos = 0, pass = 0;
  gint          cur_progress, max_progress;
  gint          v;
  gint          i, j;
  gchar        *framename;
  gchar        *framename_ptr;
  gboolean      alpha_frame = FALSE;
  static gint   previous_disposal;

  /* Guard against bogus frame size */
  if (len < 1 || height < 1)
    {
      g_message ("Bogus frame dimensions");
      return -1;
    }

  /*
   **  Initialize the Compression routines
   */
  if (! ReadOK (fd, &c, 1))
    {
      g_message ("EOF / read error on image data");
      return -1;
    }

  if (LZWReadByte (fd, TRUE, c) < 0)
    {
      g_message ("Error while reading");
      return -1;
    }

  if (frame_number == 1)
    {
      /* Guard against bogus logical screen size values */
      if (screenwidth == 0)
        screenwidth = len;

      if (screenheight == 0)
        screenheight = height;

      image_ID = gimp_image_new (screenwidth, screenheight, GIMP_INDEXED);
      gimp_image_set_filename (image_ID, filename);

      for (i = 0, j = 0; i < ncols; i++)
        {
          used_cmap[0][i] = gimp_cmap[j++] = cmap[0][i];
          used_cmap[1][i] = gimp_cmap[j++] = cmap[1][i];
          used_cmap[2][i] = gimp_cmap[j++] = cmap[2][i];
        }

      gimp_image_set_colormap (image_ID, gimp_cmap, ncols);

      if (Gif89.delayTime < 0)
        framename = g_strdup (_("Background"));
      else
        framename = g_strdup_printf (_("Background (%d%s)"),
                                     10 * Gif89.delayTime, "ms");

      previous_disposal = Gif89.disposal;

      if (Gif89.transparent == -1)
        {
          layer_ID = gimp_layer_new (image_ID, framename,
                                     len, height,
                                     GIMP_INDEXED_IMAGE, 100, GIMP_NORMAL_MODE);
        }
      else
        {
          layer_ID = gimp_layer_new (image_ID, framename,
                                     len, height,
                                     GIMP_INDEXEDA_IMAGE, 100, GIMP_NORMAL_MODE);
          alpha_frame=TRUE;
        }

      g_free (framename);
    }
  else /* NOT FIRST FRAME */
    {
      gimp_progress_set_text_printf (_("Opening '%s' (frame %d)"),
                                     gimp_filename_to_utf8 (filename),
                                     frame_number);
      gimp_progress_pulse ();

       /* If the colourmap is now different, we have to promote to RGB! */
      if (! promote_to_rgb)
        {
          for (i = 0; i < ncols; i++)
            {
              if ((used_cmap[0][i] != cmap[0][i]) ||
                  (used_cmap[1][i] != cmap[1][i]) ||
                  (used_cmap[2][i] != cmap[2][i]))
                {
                  /* Everything is RGB(A) from now on... sigh. */
                  promote_to_rgb = TRUE;

                  /* Promote everything we have so far into RGB(A) */
#ifdef GIFDEBUG
                  g_print ("GIF: Promoting image to RGB...\n");
#endif
                  gimp_image_convert_rgb (image_ID);

                  break;
                }
            }
        }

      if (Gif89.delayTime < 0)
        framename = g_strdup_printf (_("Frame %d"), frame_number);
      else
        framename = g_strdup_printf (_("Frame %d (%d%s)"),
                                     frame_number, 10 * Gif89.delayTime, "ms");

      switch (previous_disposal)
        {
        case 0x00:
          break; /* 'don't care' */
        case 0x01:
          framename_ptr = framename;
          framename = g_strconcat (framename, " (combine)", NULL);
          g_free (framename_ptr);
          break;
        case 0x02:
          framename_ptr = framename;
          framename = g_strconcat (framename, " (replace)", NULL);
          g_free (framename_ptr);
          break;
        case 0x03:  /* Rarely-used, and unhandled by many
                       loaders/players (including GIMP: we treat as
                       'combine' mode). */
          framename_ptr = framename;
          framename = g_strconcat (framename, " (combine) (!)", NULL);
          g_free (framename_ptr);
          break;
        case 0x04: /* I've seen a composite of this type. stvo_online_banner2.gif */
        case 0x05:
        case 0x06: /* I've seen a composite of this type. bn31.Gif */
        case 0x07:
          framename_ptr = framename;
          framename = g_strconcat (framename, " (unknown disposal)", NULL);
          g_free (framename_ptr);
          g_message (_("GIF: Undocumented GIF composite type %d is "
                       "not handled.  Animation might not play or "
                       "re-save perfectly."),
                     previous_disposal);
          break;
        default:
          g_message ("Disposal word got corrupted.  Bug.");
          break;
        }
      previous_disposal = Gif89.disposal;

      layer_ID = gimp_layer_new (image_ID, framename,
                                 len, height,
                                 promote_to_rgb ?
                                 GIMP_RGBA_IMAGE : GIMP_INDEXEDA_IMAGE,
                                 100, GIMP_NORMAL_MODE);
      alpha_frame = TRUE;
      g_free (framename);
    }

  frame_number++;

  gimp_image_insert_layer (image_ID, layer_ID, -1, 0);
  gimp_layer_translate (layer_ID, (gint) leftpos, (gint) toppos);

  drawable = gimp_drawable_get (layer_ID);

  cur_progress = 0;
  max_progress = height;

  if (alpha_frame)
    dest = (guchar *) g_malloc (len * height * (promote_to_rgb ? 4 : 2));
  else
    dest = (guchar *) g_malloc (len * height);

#ifdef GIFDEBUG
    g_print ("GIF: reading %d by %d%s GIF image, ncols=%d\n",
             len, height, interlace ? " interlaced" : "", ncols);
#endif

  if (! alpha_frame && promote_to_rgb)
    {
      /* I don't see how one would easily construct a GIF in which
         this could happen, but it's a mad mad world. */
      g_message ("Ouch!  Can't handle non-alpha RGB frames.\n"
                 "Please file a bug report in GIMP's bugzilla.");
      gimp_quit ();
    }

  while ((v = LZWReadByte (fd, FALSE, c)) >= 0)
    {
      if (alpha_frame)
        {
          if (((guchar) v > highest_used_index) && !(v == Gif89.transparent))
            highest_used_index = (guchar) v;

          if (promote_to_rgb)
            {
              temp = dest + ( (ypos * len) + xpos ) * 4;
              *(temp  ) = (guchar) cmap[0][v];
              *(temp+1) = (guchar) cmap[1][v];
              *(temp+2) = (guchar) cmap[2][v];
              *(temp+3) = (guchar) ((v == Gif89.transparent) ? 0 : 255);
            }
          else
            {
              temp = dest + ( (ypos * len) + xpos ) * 2;
              *temp = (guchar) v;
              *(temp+1) = (guchar) ((v == Gif89.transparent) ? 0 : 255);
            }
        }
      else
        {
          if ((guchar) v > highest_used_index)
            highest_used_index = (guchar) v;

          temp = dest + (ypos * len) + xpos;
          *temp = (guchar) v;
        }

      xpos++;
      if (xpos == len)
        {
          xpos = 0;
          if (interlace)
            {
              switch (pass)
                {
                case 0:
                case 1:
                  ypos += 8;
                  break;
                case 2:
                  ypos += 4;
                  break;
                case 3:
                  ypos += 2;
                  break;
                }

              if (ypos >= height)
                {
                  pass++;
                  switch (pass)
                    {
                    case 1:
                      ypos = 4;
                      break;
                    case 2:
                      ypos = 2;
                      break;
                    case 3:
                      ypos = 1;
                      break;
                    default:
                      goto fini;
                    }
                }
            }
          else
            {
              ypos++;
            }

          if (frame_number == 1)
            {
              cur_progress++;
              if ((cur_progress % 16) == 0)
                gimp_progress_update ((gdouble) cur_progress /
                                      (gdouble) max_progress);
            }
        }

      if (ypos >= height)
        break;
    }

 fini:
  if (LZWReadByte (fd, FALSE, c) >= 0)
    g_print ("GIF: too much input data, ignoring extra...\n");

  gimp_progress_update (1.0);
  gimp_pixel_rgn_init (&pixel_rgn, drawable,
                       0, 0, drawable->width, drawable->height, TRUE, FALSE);
  gimp_pixel_rgn_set_rect (&pixel_rgn, dest,
                           0, 0, drawable->width, drawable->height);

  g_free (dest);

  gimp_drawable_flush (drawable);
  gimp_drawable_detach (drawable);

  return image_ID;
}