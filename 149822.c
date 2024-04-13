add_length(regex_t* reg, OnigDistance len)
{
  LengthType l = (LengthType )len;

  BBUF_ADD(reg, &l, SIZE_LENGTH);
  return 0;
}