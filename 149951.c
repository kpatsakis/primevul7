resize_property_list(int new_size, const OnigCodePoint*** plist, int* psize)
{
  size_t size;
  const OnigCodePoint **list = *plist;

  size = sizeof(OnigCodePoint*) * new_size;
  if (IS_NULL(list)) {
    list = (const OnigCodePoint** )xmalloc(size);
    if (IS_NULL(list)) return ONIGERR_MEMORY;
  }
  else {
    const OnigCodePoint **tmp;
    tmp = (const OnigCodePoint** )xrealloc((void* )list, size);
    if (IS_NULL(tmp)) return ONIGERR_MEMORY;
    list = tmp;
  }

  *plist = list;
  *psize = new_size;

  return 0;
}