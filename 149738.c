onigenc_mb2_code_to_mbclen(OnigCodePoint code, OnigEncoding enc ARG_UNUSED)
{
  if (code <= 0xff) return 1;
  if (code <= 0xffff) return 2;
  return ONIGERR_TOO_BIG_WIDE_CHAR_VALUE;
}