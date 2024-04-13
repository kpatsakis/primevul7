forward_search_range(regex_t* reg, const UChar* str, const UChar* end, UChar* s,
		     UChar* range, UChar** low, UChar** high, UChar** low_prev)
{
  UChar *p, *pprev = (UChar* )NULL;

#ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr, "forward_search_range: str: %"PRIuPTR" (%p), end: %"PRIuPTR" (%p), s: %"PRIuPTR" (%p), range: %"PRIuPTR" (%p)\n",
	  (intptr_t )str, str, (intptr_t )end, end, (intptr_t )s, s, (intptr_t )range, range);
#endif

  p = s;
  if (reg->dmin > 0) {
    if (ONIGENC_IS_SINGLEBYTE(reg->enc)) {
      p += reg->dmin;
    }
    else {
      UChar *q = p + reg->dmin;
      while (p < q) p += enclen(reg->enc, p, end);
    }
  }

 retry:
  switch (reg->optimize) {
  case ONIG_OPTIMIZE_EXACT:
    p = slow_search(reg->enc, reg->exact, reg->exact_end, p, end, range);
    break;
  case ONIG_OPTIMIZE_EXACT_IC:
    p = slow_search_ic(reg->enc, reg->case_fold_flag,
		       reg->exact, reg->exact_end, p, end, range);
    break;

  case ONIG_OPTIMIZE_EXACT_BM:
    p = bm_search(reg, reg->exact, reg->exact_end, p, end, range);
    break;

  case ONIG_OPTIMIZE_EXACT_BM_NOT_REV:
    p = bm_search_notrev(reg, reg->exact, reg->exact_end, p, end, range);
    break;

  case ONIG_OPTIMIZE_EXACT_BM_IC:
    p = bm_search_ic(reg, reg->exact, reg->exact_end, p, end, range);
    break;

  case ONIG_OPTIMIZE_EXACT_BM_NOT_REV_IC:
    p = bm_search_notrev_ic(reg, reg->exact, reg->exact_end, p, end, range);
    break;

  case ONIG_OPTIMIZE_MAP:
    p = map_search(reg->enc, reg->map, p, range, end);
    break;
  }

  if (p && p < range) {
    if (p - reg->dmin < s) {
    retry_gate:
      pprev = p;
      p += enclen(reg->enc, p, end);
      goto retry;
    }

    if (reg->sub_anchor) {
      UChar* prev;

      switch (reg->sub_anchor) {
      case ANCHOR_BEGIN_LINE:
	if (!ON_STR_BEGIN(p)) {
	  prev = onigenc_get_prev_char_head(reg->enc,
					    (pprev ? pprev : str), p, end);
	  if (!ONIGENC_IS_MBC_NEWLINE_EX(reg->enc, prev, str, end, reg->options, 0))
	    goto retry_gate;
	}
	break;

      case ANCHOR_END_LINE:
	if (ON_STR_END(p)) {
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
	  prev = (UChar* )onigenc_get_prev_char_head(reg->enc,
					    (pprev ? pprev : str), p);
	  if (prev && ONIGENC_IS_MBC_NEWLINE_EX(reg->enc, prev, str, end, reg->options, 1))
	    goto retry_gate;
#endif
	}
	else if (! ONIGENC_IS_MBC_NEWLINE_EX(reg->enc, p, str, end, reg->options, 1))
	  goto retry_gate;
	break;
      }
    }

    if (reg->dmax == 0) {
      *low = p;
      if (low_prev) {
	if (*low > s)
	  *low_prev = onigenc_get_prev_char_head(reg->enc, s, p, end);
	else
	  *low_prev = onigenc_get_prev_char_head(reg->enc,
						 (pprev ? pprev : str), p, end);
      }
    }
    else {
      if (reg->dmax != ONIG_INFINITE_DISTANCE) {
	*low = p - reg->dmax;
	if (*low > s) {
	  *low = onigenc_get_right_adjust_char_head_with_prev(reg->enc, s,
							      *low, end, (const UChar** )low_prev);
	  if (low_prev && IS_NULL(*low_prev))
	    *low_prev = onigenc_get_prev_char_head(reg->enc,
						   (pprev ? pprev : s), *low, end);
	}
	else {
	  if (low_prev)
	    *low_prev = onigenc_get_prev_char_head(reg->enc,
					       (pprev ? pprev : str), *low, end);
	}
      }
    }
    /* no needs to adjust *high, *high is used as range check only */
    *high = p - reg->dmin;

#ifdef ONIG_DEBUG_SEARCH
    fprintf(stderr,
    "forward_search_range success: low: %"PRIdPTR", high: %"PRIdPTR", dmin: %"PRIdPTR", dmax: %"PRIdPTR"\n",
	    *low - str, *high - str, reg->dmin, reg->dmax);
#endif
    return 1; /* success */
  }

  return 0; /* fail */
}