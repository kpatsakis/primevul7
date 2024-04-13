add_char_amb_opt_map_info(OptMapInfo* map, UChar* p, UChar* end,
                          OnigEncoding enc, OnigCaseFoldType case_fold_flag)
{
  OnigCaseFoldCodeItem items[ONIGENC_GET_CASE_FOLD_CODES_MAX_NUM];
  UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN];
  int i, n;

  add_char_opt_map_info(map, p[0], enc);

  case_fold_flag = DISABLE_CASE_FOLD_MULTI_CHAR(case_fold_flag);
  n = ONIGENC_GET_CASE_FOLD_CODES_BY_STR(enc, case_fold_flag, p, end, items);
  if (n < 0) return n;

  for (i = 0; i < n; i++) {
    ONIGENC_CODE_TO_MBC(enc, items[i].code[0], buf);
    add_char_opt_map_info(map, buf[0], enc);
  }

  return 0;
}