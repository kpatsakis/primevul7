onigenc_mbn_mbc_to_code(OnigEncoding enc, const UChar* p, const UChar* end)
{
  int c, i, len;
  OnigCodePoint n;

  len = enclen(enc, p, end);
  n = (OnigCodePoint )(*p++);
  if (len == 1) return n;

  for (i = 1; i < len; i++) {
    if (p >= end) break;
    c = *p++;
    n <<= 8;  n += c;
  }
  return n;
}