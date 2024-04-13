onig_scan(regex_t* reg, const UChar* str, const UChar* end,
	  OnigRegion* region, OnigOptionType option,
	  int (*scan_callback)(OnigPosition, OnigPosition, OnigRegion*, void*),
	  void* callback_arg)
{
  OnigPosition r;
  OnigPosition n;
  int rs;
  const UChar* start;

  n = 0;
  start = str;
  while (1) {
    r = onig_search(reg, str, end, start, end, region, option);
    if (r >= 0) {
      rs = scan_callback(n, r, region, callback_arg);
      n++;
      if (rs != 0)
	return rs;

      if (region->end[0] == start - str)
	start++;
      else
	start = str + region->end[0];

      if (start > end)
	break;
    }
    else if (r == ONIG_MISMATCH) {
      break;
    }
    else { /* error */
      return r;
    }
  }

  return n;
}