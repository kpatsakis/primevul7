onigenc_with_ascii_strnicmp(OnigEncoding enc, const UChar* p, const UChar* end,
                            const UChar* sascii /* ascii */, int n)
{
  int x, c;

  while (n-- > 0) {
    if (p >= end) return (int )(*sascii);

    c = (int )ONIGENC_MBC_TO_CODE(enc, p, end);
    if (ONIGENC_IS_ASCII_CODE(c))
      c = ONIGENC_ASCII_CODE_TO_LOWER_CASE(c);
    x = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*sascii) - c;
    if (x) return x;

    sascii++;
    p += enclen(enc, p, end);
  }
  return 0;
}