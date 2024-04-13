onigenc_get_right_adjust_char_head_with_prev(OnigEncoding enc,
				   const UChar* start, const UChar* s, const UChar* end, const UChar** prev)
{
  UChar* p = ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, start, s, end);

  if (p < s) {
    if (prev) *prev = (const UChar* )p;
    p += enclen(enc, p, end);
  }
  else {
    if (prev) *prev = (const UChar* )NULL; /* Sorry */
  }
  return p;
}