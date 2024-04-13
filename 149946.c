onigenc_single_byte_left_adjust_char_head(const UChar* start ARG_UNUSED,
					  const UChar* s,
					  const UChar* end ARG_UNUSED,
					  OnigEncoding enc ARG_UNUSED)
{
  return (UChar* )s;
}