Gif_ReadFile(FILE *f)
{
  return Gif_FullReadFile(f, GIF_READ_UNCOMPRESSED, 0, 0);
}