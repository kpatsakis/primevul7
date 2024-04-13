static int IsDigit(int enc ARG_UNUSED, int c)
{
  if (c >= 0x30 && c <= 0x39) return 1;
  return 0;
}