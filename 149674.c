onigenc_strlen_null(OnigEncoding enc, const UChar* s)
{
  int n = 0;
  UChar* p = (UChar* )s;
  UChar* e;

  while (1) {
    if (*p == '\0') {
      UChar* q;
      int len = ONIGENC_MBC_MINLEN(enc);

      if (len == 1) return n;
      q = p + 1;
      while (len > 1) {
        if (*q != '\0') break;
        q++;
        len--;
      }
      if (len == 1) return n;
    }
    e = p + ONIGENC_MBC_MAXLEN(enc);
    p += ONIGENC_MBC_ENC_LEN(enc, p, e);
    n++;
  }
}