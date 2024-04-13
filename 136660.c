XML_MemRealloc(XML_Parser parser, void *ptr, size_t size)
{
  return REALLOC(ptr, size);
}