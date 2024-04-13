onigenc_unicode_mbc_case_fold(OnigEncoding enc,
    OnigCaseFoldType flag ARG_UNUSED, const UChar** pp, const UChar* end,
    UChar* fold)
{
  const CodePointList3 *to;
  OnigCodePoint code;
  int i, len, rlen;
  const UChar *p = *pp;

  code = ONIGENC_MBC_TO_CODE(enc, p, end);
  len = enclen(enc, p, end);
  *pp += len;

#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
  if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) != 0) {
    if (code == 'I') {
      return ONIGENC_CODE_TO_MBC(enc, DOTLESS_i, fold);
    }
    else if (code == I_WITH_DOT_ABOVE) {
      return ONIGENC_CODE_TO_MBC(enc, 'i', fold);
    }
  }
#endif

  if ((to = onigenc_unicode_fold_lookup(code)) != 0) {
    if (OnigCodePointCount(to->n) == 1) {
      return ONIGENC_CODE_TO_MBC(enc, to->code[0], fold);
    }
#if 0
    /* NO NEEDS TO CHECK */
    else if ((flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) != 0)
#else
    else
#endif
    {
      rlen = 0;
      for (i = 0; i < OnigCodePointCount(to->n); i++) {
	len = ONIGENC_CODE_TO_MBC(enc, to->code[i], fold);
	fold += len;
	rlen += len;
      }
      return rlen;
    }
  }

  for (i = 0; i < len; i++) {
    *fold++ = *p++;
  }
  return len;
}