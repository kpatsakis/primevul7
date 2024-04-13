Gif_FullUncompressImage(Gif_Stream* gfs, Gif_Image* gfi,
                        Gif_ReadErrorHandler h)
{
  Gif_Context gfc;
  Gif_Reader grr;
  int ok = 0;

  /* return right away if image is already uncompressed. this might screw over
     people who expect re-uncompressing to restore the compressed version. */
  if (gfi->img)
    return 2;
  if (gfi->image_data)
    /* we have uncompressed data, but not an 'img' array;
       this shouldn't happen */
    return 0;

  gfc.stream = gfs;
  gfc.gfi = gfi;
  gfc.prefix = Gif_NewArray(Gif_Code, GIF_MAX_CODE);
  gfc.suffix = Gif_NewArray(uint8_t, GIF_MAX_CODE);
  gfc.length = Gif_NewArray(uint16_t, GIF_MAX_CODE);
  gfc.handler = h;
  gfc.errors[0] = gfc.errors[1] = 0;

  if (gfc.prefix && gfc.suffix && gfc.length && gfi->compressed) {
    make_data_reader(&grr, gfi->compressed, gfi->compressed_len);
    ok = uncompress_image(&gfc, gfi, &grr);
  }

  Gif_DeleteArray(gfc.prefix);
  Gif_DeleteArray(gfc.suffix);
  Gif_DeleteArray(gfc.length);
  if (gfc.errors[0] || gfc.errors[1])
      gif_read_error(&gfc, -1, 0);
  return ok && !gfc.errors[1];
}