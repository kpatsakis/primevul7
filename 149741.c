static int IsAscii(int enc ARG_UNUSED, int c)
{
  if (c >= 0x00 && c <= 0x7f) return 1;
  return 0;
}