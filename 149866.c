onigenc_property_list_add_property(UChar* name, const OnigCodePoint* prop,
     hash_table_type **table, const OnigCodePoint*** plist, int *pnum,
     int *psize)
{
#define PROP_INIT_SIZE     16

  int r;

  if (*psize <= *pnum) {
    int new_size = (*psize == 0 ? PROP_INIT_SIZE : *psize * 2);
    r = resize_property_list(new_size, plist, psize);
    if (r != 0) return r;
  }

  (*plist)[*pnum] = prop;

  if (ONIG_IS_NULL(*table)) {
    *table = onig_st_init_strend_table_with_size(PROP_INIT_SIZE);
    if (ONIG_IS_NULL(*table)) return ONIGERR_MEMORY;
  }

  *pnum = *pnum + 1;
  onig_st_insert_strend(*table, name, name + strlen((char* )name),
			(hash_data_type )(*pnum + ONIGENC_MAX_STD_CTYPE));
  return 0;
}