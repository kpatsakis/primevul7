add_rel_addr(regex_t* reg, int addr)
{
  RelAddrType ra = (RelAddrType )addr;

  BBUF_ADD(reg, &ra, SIZE_RELADDR);
  return 0;
}