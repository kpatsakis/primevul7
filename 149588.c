add_bytes(regex_t* reg, UChar* bytes, OnigDistance len)
{
  BBUF_ADD(reg, bytes, len);
  return 0;
}