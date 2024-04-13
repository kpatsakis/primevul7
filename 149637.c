onigenc_step(OnigEncoding enc, const UChar* p, const UChar* end, int n)
{
  UChar* q = (UChar* )p;
  while (n-- > 0) {
    q += ONIGENC_MBC_ENC_LEN(enc, q, end);
  }
  return (q <= end ? q : NULL);
}