unsigned x3f_sget4(uchar *s)
{
  return s[0] | s[1] << 8 | s[2] << 16 | s[3] << 24;
}