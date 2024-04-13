onig_search(regex_t* reg, const UChar* str, const UChar* end,
	    const UChar* start, const UChar* range, OnigRegion* region, OnigOptionType option)
{
  return onig_search_gpos(reg, str, end, start, start, range, region, option);
}