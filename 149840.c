bits_of(const OnigCodePoint c, const int n)
{
  return (c >> (2 - n) * 7) & 127;
}