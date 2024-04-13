onigenc_with_ascii_strncmp(OnigEncoding enc, const UChar* p, const UChar* end,
                           const UChar* sascii /* ascii */, int n)
{
  int x, c;

  while (n-- > 0) {
    if (p >= end) return (int )(*sascii);

    c = (int )ONIGENC_MBC_TO_CODE(enc, p, end);
    x = *sascii - c;
    if (x) return x;

    sascii++;
    p += enclen(enc, p, end);
  }
  return 0;
}