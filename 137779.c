one_code(Gif_Context *gfc, Gif_Code code)
{
  uint8_t *suffixes = gfc->suffix;
  Gif_Code *prefixes = gfc->prefix;
  uint8_t *ptr;
  int lastsuffix = 0;
  int codelength = gfc->length[code];

  gfc->decodepos += codelength;
  ptr = gfc->image + gfc->decodepos;
  while (codelength > 0) {
      lastsuffix = suffixes[code];
      code = prefixes[code];
      --ptr;
      if (ptr < gfc->maximage)
          *ptr = lastsuffix;
      --codelength;
  }

  /* return the first pixel in the code, which, since we walked backwards
     through the code, was the last suffix we processed. */
  return lastsuffix;
}