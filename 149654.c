onigenc_get_prev_char_head(OnigEncoding enc, const UChar* start, const UChar* s, const UChar* end)
{
  if (s <= start)
    return (UChar* )NULL;

  return ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, start, s - 1, end);
}