bm_search_notrev_ic(regex_t* reg, const UChar* target, const UChar* target_end,
		    const UChar* text, const UChar* text_end,
		    const UChar* text_range)
{
  const UChar *s, *se, *t, *end;
  const UChar *tail;
  ptrdiff_t skip, tlen1;
  OnigEncoding enc = reg->enc;
  int case_fold_flag = reg->case_fold_flag;

# ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr, "bm_search_notrev_ic: text: %d (%p), text_end: %d (%p), text_range: %d (%p)\n",
	  (int )text, text, (int )text_end, text_end, (int )text_range, text_range);
# endif

  tail = target_end - 1;
  tlen1 = tail - target;
  end = text_range;
  if (end + tlen1 > text_end)
    end = text_end - tlen1;

  s = text;

  if (IS_NULL(reg->int_map)) {
    while (s < end) {
      se = s + tlen1;
      if (str_lower_case_match(enc, case_fold_flag, target, target_end,
			       s, se + 1))
	return (UChar* )s;
      skip = reg->map[*se];
      t = s;
      do {
	s += enclen(reg->enc, s, end);
      } while ((s - t) < skip && s < end);
    }
  }
  else {
# if OPT_EXACT_MAXLEN >= ONIG_CHAR_TABLE_SIZE
    while (s < end) {
      se = s + tlen1;
      if (str_lower_case_match(enc, case_fold_flag, target, target_end,
			       s, se + 1))
	return (UChar* )s;
      skip = reg->int_map[*se];
      t = s;
      do {
	s += enclen(reg->enc, s, end);
      } while ((s - t) < skip && s < end);
    }
# endif
  }

  return (UChar* )NULL;
}