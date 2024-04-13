static int IsNewline(int enc ARG_UNUSED, int c)
{
  if (c == 0x0a) return 1;
  return 0;
}