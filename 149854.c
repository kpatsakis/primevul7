mbc_case_fold(OnigCaseFoldType flag,
	      const UChar** pp, const UChar* end ARG_UNUSED, UChar* lower,
	      OnigEncoding enc ARG_UNUSED)
{
  const UChar* p = *pp;

  *lower = ENC_CP1253_TO_LOWER_CASE(*p);
  (*pp)++;
  return 1;
}