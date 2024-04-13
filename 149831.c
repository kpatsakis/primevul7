add_code_range0(BBuf** pbuf, ScanEnv* env, OnigCodePoint from, OnigCodePoint to, int checkdup)
{
  if (from > to) {
    if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC))
      return 0;
    else
      return ONIGERR_EMPTY_RANGE_IN_CHAR_CLASS;
  }

  return add_code_range_to_buf0(pbuf, env, from, to, checkdup);
}