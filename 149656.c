static int IsXDigit(int enc, int c)
{
  if (enc == ASCII)
    return isxdigit(c);

  if (c >= 0x30 && c <= 0x39) return 1;
  if (c >= 0x41 && c <= 0x46) return 1;
  if (c >= 0x61 && c <= 0x66) return 1;
  return 0;
}