read_logical_screen_descriptor(Gif_Stream *gfs, Gif_Reader *grr)
     /* returns 0 on memory error */
{
  uint8_t packed;

  /* we don't care about logical screen width or height */
  gfs->screen_width = gifgetunsigned(grr);
  gfs->screen_height = gifgetunsigned(grr);

  packed = gifgetbyte(grr);
  gfs->background = gifgetbyte(grr);

  /* don't care about pixel aspect ratio */
  gifgetbyte(grr);

  if (packed & 0x80) { /* have a global color table */
      int ncol = 1 << ((packed & 0x07) + 1);
      gfs->global = read_color_table(ncol, grr);
      if (!gfs->global) return 0;
      gfs->global->refcount = 1;
  } else
      gfs->background = 256;

  return 1;
}