onigenc_str_bytelen_null(OnigEncoding enc, const UChar* s)
{
  UChar* start = (UChar* )s;
  UChar* p = (UChar* )s;
  UChar* e;

  while (1) {
    if (*p == '\0') {
      UChar* q;
      int len = ONIGENC_MBC_MINLEN(enc);

      if (len == 1) return (int )(p - start);
      q = p + 1;
      while (len > 1) {
        if (*q != '\0') break;
        q++;
        len--;
      }
      if (len == 1) return (int )(p - start);
    }
    e = p + ONIGENC_MBC_MAXLEN(enc);
    p += ONIGENC_MBC_ENC_LEN(enc, p, e);
  }
}