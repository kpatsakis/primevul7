void CLASS phase_one_load_raw_c()
{
  static const int length[] = { 8,7,6,9,11,10,5,12,14,13 };
  int *offset, len[2], pred[2], row, col, i, j;
  ushort *pixel;
  short (*c_black)[2], (*r_black)[2];
#ifdef LIBRAW_LIBRARY_BUILD
  if(ph1.format == 6)
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
#endif

  pixel = (ushort *) calloc (raw_width*3 + raw_height*4, 2);
  merror (pixel, "phase_one_load_raw_c()");
  offset = (int *) (pixel + raw_width);
  fseek (ifp, strip_offset, SEEK_SET);
  for (row=0; row < raw_height; row++)
    offset[row] = get4();
  c_black = (short (*)[2]) (offset + raw_height);
  fseek (ifp, ph1.black_col, SEEK_SET);
  if (ph1.black_col)
      read_shorts ((ushort *) c_black[0], raw_height*2);
  r_black = c_black + raw_height;
  fseek (ifp, ph1.black_row, SEEK_SET);
  if (ph1.black_row)
      read_shorts ((ushort *) r_black[0], raw_width*2);

#ifdef LIBRAW_LIBRARY_BUILD
  // Copy data to internal copy (ever if not read)
  if (ph1.black_col || ph1.black_row )
    {
      imgdata.rawdata.ph1_cblack = (short(*)[2])calloc(raw_height*2,sizeof(ushort));
      merror(imgdata.rawdata.ph1_cblack,"phase_one_load_raw_c()");
      memmove(imgdata.rawdata.ph1_cblack,(ushort*)c_black[0],raw_height*2*sizeof(ushort));
      imgdata.rawdata.ph1_rblack = (short(*)[2])calloc(raw_width*2,sizeof(ushort));
      merror(imgdata.rawdata.ph1_rblack,"phase_one_load_raw_c()");
      memmove(imgdata.rawdata.ph1_rblack,(ushort*)r_black[0],raw_width*2*sizeof(ushort));
    }
#endif

  for (i=0; i < 256; i++)
    curve[i] = i*i / 3.969 + 0.5;
#ifdef LIBRAW_LIBRARY_BUILD
  try {
#endif
  for (row=0; row < raw_height; row++) {
#ifdef LIBRAW_LIBRARY_BUILD
    checkCancel();
#endif
    fseek (ifp, data_offset + offset[row], SEEK_SET);
    ph1_bits(-1);
    pred[0] = pred[1] = 0;
    for (col=0; col < raw_width; col++) {
      if (col >= (raw_width & -8))
	len[0] = len[1] = 14;
      else if ((col & 7) == 0)
	for (i=0; i < 2; i++) {
	  for (j=0; j < 5 && !ph1_bits(1); j++);
	  if (j--) len[i] = length[j*2 + ph1_bits(1)];
	}
      if ((i = len[col & 1]) == 14)
	pixel[col] = pred[col & 1] = ph1_bits(16);
      else
	pixel[col] = pred[col & 1] += ph1_bits(i) + 1 - (1 << (i - 1));
      if (pred[col & 1] >> 16) derror();
      if (ph1.format == 5 && pixel[col] < 256)
	pixel[col] = curve[pixel[col]];
    }
    for (col=0; col < raw_width; col++) {
#ifndef LIBRAW_LIBRARY_BUILD
      i = (pixel[col] << 2) - ph1.t_black
	+ c_black[row][col >= ph1.split_col]
	+ r_black[col][row >= ph1.split_row];
      if (i > 0) RAW(row,col) = i;
#else
      RAW(row,col) = pixel[col] << 2;
#endif
    }
  }
#ifdef LIBRAW_LIBRARY_BUILD
  } catch(...) {
    free (pixel);
    throw;
  }
#endif
  free (pixel);
  maximum = 0xfffc - ph1.t_black;
}