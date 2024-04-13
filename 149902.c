unset_addr_list_init(UnsetAddrList* uslist, int size)
{
  UnsetAddr* p;

  p = (UnsetAddr* )xmalloc(sizeof(UnsetAddr)* size);
  CHECK_NULL_RETURN_MEMERR(p);
  uslist->num   = 0;
  uslist->alloc = size;
  uslist->us    = p;
  return 0;
}