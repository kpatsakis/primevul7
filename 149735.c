add_pointer(regex_t* reg, void* addr)
{
  PointerType ptr = (PointerType )addr;

  BBUF_ADD(reg, &ptr, SIZE_POINTER);
  return 0;
}