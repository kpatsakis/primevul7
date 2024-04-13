onigenc_ascii_is_mbc_ambiguous(OnigCaseFoldType flag ARG_UNUSED,
			       const UChar** pp, const UChar* end ARG_UNUSED)
{
  const UChar* p = *pp;

  (*pp)++;
  return ONIGENC_IS_ASCII_CODE_CASE_AMBIG(*p);
}