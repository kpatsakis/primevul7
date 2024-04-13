set_bm_skip(UChar* s, UChar* end, regex_t* reg,
	    UChar skip[], int** int_skip, int ignore_case)
{
  OnigDistance i, len;
  int clen, flen, n, j, k;
  UChar *p, buf[ONIGENC_GET_CASE_FOLD_CODES_MAX_NUM][ONIGENC_MBC_CASE_FOLD_MAXLEN];
  OnigCaseFoldCodeItem items[ONIGENC_GET_CASE_FOLD_CODES_MAX_NUM];
  OnigEncoding enc = reg->enc;

  len = end - s;
  if (len < ONIG_CHAR_TABLE_SIZE) {
    for (i = 0; i < ONIG_CHAR_TABLE_SIZE; i++) skip[i] = (UChar )(len + 1);

    n = 0;
    for (i = 0; i < len; i += clen) {
      p = s + i;
      if (ignore_case)
	n = ONIGENC_GET_CASE_FOLD_CODES_BY_STR(enc, reg->case_fold_flag,
					       p, end, items);
      clen = enclen(enc, p, end);

      for (j = 0; j < n; j++) {
	if ((items[j].code_len != 1) || (items[j].byte_len != clen))
	  return 1;  /* different length isn't supported. */
	flen = ONIGENC_CODE_TO_MBC(enc, items[j].code[0], buf[j]);
	if (flen != clen)
	  return 1;  /* different length isn't supported. */
      }
      for (j = 0; j < clen; j++) {
	skip[s[i + j]] = (UChar )(len - i - j);
	for (k = 0; k < n; k++) {
	  skip[buf[k][j]] = (UChar )(len - i - j);
	}
      }
    }
  }
  else {
# if OPT_EXACT_MAXLEN < ONIG_CHAR_TABLE_SIZE
    /* This should not happen. */
    return ONIGERR_TYPE_BUG;
# else
    if (IS_NULL(*int_skip)) {
      *int_skip = (int* )xmalloc(sizeof(int) * ONIG_CHAR_TABLE_SIZE);
      if (IS_NULL(*int_skip)) return ONIGERR_MEMORY;
    }
    for (i = 0; i < ONIG_CHAR_TABLE_SIZE; i++) (*int_skip)[i] = (int )(len + 1);

    n = 0;
    for (i = 0; i < len; i += clen) {
      p = s + i;
      if (ignore_case)
	n = ONIGENC_GET_CASE_FOLD_CODES_BY_STR(enc, reg->case_fold_flag,
					       p, end, items);
      clen = enclen(enc, p, end);

      for (j = 0; j < n; j++) {
	if ((items[j].code_len != 1) || (items[j].byte_len != clen))
	  return 1;  /* different length isn't supported. */
	flen = ONIGENC_CODE_TO_MBC(enc, items[j].code[0], buf[j]);
	if (flen != clen)
	  return 1;  /* different length isn't supported. */
      }
      for (j = 0; j < clen; j++) {
	(*int_skip)[s[i + j]] = (int )(len - i - j);
	for (k = 0; k < n; k++) {
	  (*int_skip)[buf[k][j]] = (int )(len - i - j);
	}
      }
    }
# endif
  }
  return 0;
}