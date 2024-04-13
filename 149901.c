onigenc_strlen(OnigEncoding enc, const UChar* p, const UChar* end)
{
  int n = 0;
  UChar* q = (UChar* )p;

  while (q < end) {
    q += ONIGENC_MBC_ENC_LEN(enc, q, end);
    n++;
  }
  return n;
}