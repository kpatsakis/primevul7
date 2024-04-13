gifgetunsigned(Gif_Reader *grr)
{
  uint8_t one = gifgetbyte(grr);
  uint8_t two = gifgetbyte(grr);
  return one | (two << 8);
}