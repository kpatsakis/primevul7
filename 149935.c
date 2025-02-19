onig_new(regex_t** reg, const UChar* pattern, const UChar* pattern_end,
	  OnigOptionType option, OnigEncoding enc, const OnigSyntaxType* syntax,
	  OnigErrorInfo* einfo)
{
  int r;

  *reg = (regex_t* )xmalloc(sizeof(regex_t));
  if (IS_NULL(*reg)) return ONIGERR_MEMORY;

  r = onig_reg_init(*reg, option, ONIGENC_CASE_FOLD_DEFAULT, enc, syntax);
  if (r) goto err;

  r = onig_compile(*reg, pattern, pattern_end, einfo);
  if (r) {
  err:
    onig_free(*reg);
    *reg = NULL;
  }
  return r;
}