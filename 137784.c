read_application_extension(Gif_Context *gfc, Gif_Reader *grr)
{
  Gif_Stream *gfs = gfc->stream;
  uint8_t buffer[GIF_MAX_BLOCK + 1];
  uint8_t len = gifgetbyte(grr);
  gifgetblock(buffer, len, grr);

  /* Read the Netscape loop extension. */
  if (len == 11
      && (memcmp(buffer, "NETSCAPE2.0", 11) == 0
          || memcmp(buffer, "ANIMEXTS1.0", 11) == 0)) {

    len = gifgetbyte(grr);
    if (len == 3) {
      gifgetbyte(grr); /* throw away the 1 */
      gfs->loopcount = gifgetunsigned(grr);
      len = gifgetbyte(grr);
      if (len)
	gif_read_error(gfc, 1, "bad loop extension");
    } else
      gif_read_error(gfc, 1, "bad loop extension");

    while (len > 0) {
      gifgetblock(buffer, len, grr);
      len = gifgetbyte(grr);
    }
    return 1;

  } else
    return read_unknown_extension(gfc, grr, 0xFF, (char*)buffer, len);
}