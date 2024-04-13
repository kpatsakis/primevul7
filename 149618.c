onigenc_apply_all_case_fold_with_map(int map_size,
    const OnigPairCaseFoldCodes map[],
    int ess_tsett_flag, OnigCaseFoldType flag,
    OnigApplyAllCaseFoldFunc f, void* arg)
{
  OnigCodePoint code;
  int i, r;

  r = onigenc_ascii_apply_all_case_fold(flag, f, arg, 0);
  if (r != 0) return r;

  for (i = 0; i < map_size; i++) {
    code = map[i].to;
    r = (*f)(map[i].from, &code, 1, arg);
    if (r != 0) return r;

    code = map[i].from;
    r = (*f)(map[i].to, &code, 1, arg);
    if (r != 0) return r;
  }

  if (ess_tsett_flag != 0)
    return ss_apply_all_case_fold(flag, f, arg);

  return 0;
}