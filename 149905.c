add_mem_num(regex_t* reg, int num)
{
  MemNumType n = (MemNumType )num;

  BBUF_ADD(reg, &n, SIZE_MEMNUM);
  return 0;
}