onigenc_single_byte_mbc_to_code(const UChar* p, const UChar* end ARG_UNUSED,
				OnigEncoding enc ARG_UNUSED)
{
  return (OnigCodePoint )(*p);
}