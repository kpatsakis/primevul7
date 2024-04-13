onigenc_step_back(OnigEncoding enc, const UChar* start, const UChar* s, const UChar* end, int n)
{
  while (ONIG_IS_NOT_NULL(s) && n-- > 0) {
    if (s <= start)
      return (UChar* )NULL;

    s = ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, start, s - 1, end);
  }
  return (UChar* )s;
}