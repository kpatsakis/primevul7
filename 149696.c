onigenc_mb4_code_to_mbclen(OnigCodePoint code, OnigEncoding enc ARG_UNUSED)
{
       if ((code & 0xff000000) != 0) return 4;
  else if ((code & 0xff0000) != 0) return 3;
  else if ((code & 0xff00) != 0) return 2;
  else return 1;
}