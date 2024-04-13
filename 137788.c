read_image(Gif_Reader *grr, Gif_Context *gfc, Gif_Image *gfi, int read_flags)
     /* returns 0 on memory error */
{
  uint8_t packed;

  gfi->left = gifgetunsigned(grr);
  gfi->top = gifgetunsigned(grr);
  gfi->width = gifgetunsigned(grr);
  gfi->height = gifgetunsigned(grr);
  /* Mainline GIF processors (Firefox, etc.) process missing width (height)
     as screen_width (screen_height). */
  if (gfi->width == 0)
      gfi->width = gfc->stream->screen_width;
  if (gfi->height == 0)
      gfi->height = gfc->stream->screen_height;
  /* If still zero, error. */
  if (gfi->width == 0 || gfi->height == 0) {
      gif_read_error(gfc, 1, "image has zero width and/or height");
      Gif_MakeImageEmpty(gfi);
      read_flags = 0;
  }
  /* If position out of range, error. */
  if ((unsigned) gfi->left + (unsigned) gfi->width > 0xFFFF
      || (unsigned) gfi->top + (unsigned) gfi->height > 0xFFFF) {
      gif_read_error(gfc, 1, "image position and/or dimensions out of range");
      Gif_MakeImageEmpty(gfi);
      read_flags = 0;
  }
  GIF_DEBUG(("<%ux%u> ", gfi->width, gfi->height));

  packed = gifgetbyte(grr);
  if (packed & 0x80) { /* have a local color table */
    int ncol = 1 << ((packed & 0x07) + 1);
    gfi->local = read_color_table(ncol, grr);
    if (!gfi->local) return 0;
    gfi->local->refcount = 1;
  }

  gfi->interlace = (packed & 0x40) != 0;

  /* Keep the compressed data if asked */
  if (read_flags & GIF_READ_COMPRESSED) {
    if (!read_compressed_image(gfi, grr, read_flags))
      return 0;
    if (read_flags & GIF_READ_UNCOMPRESSED) {
      Gif_Reader new_grr;
      make_data_reader(&new_grr, gfi->compressed, gfi->compressed_len);
      if (!uncompress_image(gfc, gfi, &new_grr))
	return 0;
    }

  } else if (read_flags & GIF_READ_UNCOMPRESSED) {
    if (!uncompress_image(gfc, gfi, grr))
      return 0;

  } else {
    /* skip over the image */
    uint8_t buffer[GIF_MAX_BLOCK];
    int i = gifgetbyte(grr);
    while (i > 0) {
      gifgetblock(buffer, i, grr);
      i = gifgetbyte(grr);
    }
  }

  return 1;
}