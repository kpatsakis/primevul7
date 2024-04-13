strdup_with_null(OnigEncoding enc, UChar* s, UChar* end)
{
  ptrdiff_t slen;
  int term_len, i;
  UChar *r;

  slen = end - s;
  term_len = ONIGENC_MBC_MINLEN(enc);

  r = (UChar* )xmalloc(slen + term_len);
  CHECK_NULL_RETURN(r);
  xmemcpy(r, s, slen);

  for (i = 0; i < term_len; i++)
    r[slen + i] = (UChar )0;

  return r;
}