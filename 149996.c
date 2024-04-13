bm_search(regex_t* reg, const UChar* target, const UChar* target_end,
	  const UChar* text, const UChar* text_end, const UChar* text_range)
{
  const UChar *s, *t, *p, *end;
  const UChar *tail;
  ptrdiff_t tlen1;

# ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr, "bm_search: text: %"PRIuPTR", text_end: %"PRIuPTR", text_range: %"PRIuPTR"\n",
	  text, text_end, text_range);
# endif

  tail = target_end - 1;
  tlen1 = tail - target;
  end = text_range + tlen1;
  if (end > text_end)
    end = text_end;

  s = text + tlen1;
  if (IS_NULL(reg->int_map)) {
    while (s < end) {
      p = s;
      t = tail;
      while (*p == *t) {
	if (t == target) return (UChar* )p;
	p--; t--;
      }
      if (s + 1 >= end) break;
      s += reg->map[s[1]];
    }
  }
  else { /* see int_map[] */
# if OPT_EXACT_MAXLEN >= ONIG_CHAR_TABLE_SIZE
    while (s < end) {
      p = s;
      t = tail;
      while (*p == *t) {
	if (t == target) return (UChar* )p;
	p--; t--;
      }
      if (s + 1 >= end) break;
      s += reg->int_map[s[1]];
    }
# endif
  }
  return (UChar* )NULL;
}