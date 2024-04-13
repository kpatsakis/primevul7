ss_apply_all_case_fold(OnigCaseFoldType flag ARG_UNUSED,
		       OnigApplyAllCaseFoldFunc f, void* arg)
{
  OnigCodePoint ss[] = { 0x73, 0x73 };

  return (*f)((OnigCodePoint )0xdf, ss, 2, arg);
}