code2_equal(const OnigCodePoint *x, const OnigCodePoint *y)
{
  if (x[0] != y[0]) return 0;
  if (x[1] != y[1]) return 0;
  return 1;
}