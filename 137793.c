read_graphic_control_extension(Gif_Context *gfc, Gif_Image *gfi,
			       Gif_Reader *grr)
{
  uint8_t len;
  uint8_t crap[GIF_MAX_BLOCK];

  len = gifgetbyte(grr);

  if (len == 4) {
    uint8_t packed = gifgetbyte(grr);
    gfi->disposal = (packed >> 2) & 0x07;
    gfi->delay = gifgetunsigned(grr);
    gfi->transparent = gifgetbyte(grr);
    if (!(packed & 0x01)) /* transparent color doesn't exist */
      gfi->transparent = -1;
    len -= 4;
  }

  if (len > 0) {
    gif_read_error(gfc, 1, "bad graphic extension");
    gifgetblock(crap, len, grr);
  }

  len = gifgetbyte(grr);
  while (len > 0) {
    gif_read_error(gfc, 1, "bad graphic extension");
    gifgetblock(crap, len, grr);
    len = gifgetbyte(grr);
  }
}