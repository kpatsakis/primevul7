onigenc_ascii_is_code_ctype(OnigCodePoint code, unsigned int ctype,
                            OnigEncoding enc ARG_UNUSED)
{
  if (code < 128)
    return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
  else
    return FALSE;
}