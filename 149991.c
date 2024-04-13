bm_search_ic(regex_t* reg, const UChar* target, const UChar* target_end,
	     const UChar* text, const UChar* text_end, const UChar* text_range)
{
  const UChar *s, *p, *end;
  const UChar *tail;
  ptrdiff_t tlen1;
  OnigEncoding enc = reg->enc;
  int case_fold_flag = reg->case_fold_flag;

# ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr, "bm_search_ic: text: %"PRIdPTR" (%p), text_end: %"PRIdPTR" (%p), text_range: %"PRIdPTR" (%p)\n",
	  (intptr_t )text, text, (intptr_t )text_end, text_end, (intptr_t )text_range, text_range);
# endif

  tail = target_end - 1;
  tlen1 = tail - target;
  end = text_range + tlen1;
  if (end > text_end)
    end = text_end;

  s = text + tlen1;
  if (IS_NULL(reg->int_map)) {
    while (s < end) {
      p = s - tlen1;
      if (str_lower_case_match(enc, case_fold_flag, target, target_end,
			       p, s + 1))
	return (UChar* )p;
      if (s + 1 >= end) break;
      s += reg->map[s[1]];
    }
  }
  else { /* see int_map[] */
# if OPT_EXACT_MAXLEN >= ONIG_CHAR_TABLE_SIZE
    while (s < end) {
      p = s - tlen1;
      if (str_lower_case_match(enc, case_fold_flag, target, target_end,
			       p, s + 1))
	return (UChar* )p;
      if (s + 1 >= end) break;
      s += reg->int_map[s[1]];
    }
# endif
  }
  return (UChar* )NULL;
}