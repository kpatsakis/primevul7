onig_st_init_strend_table_with_size(st_index_t size)
{
  static const struct st_hash_type hashType = {
    str_end_cmp,
    str_end_hash,
  };

  return (hash_table_type* )
           onig_st_init_table_with_size(&hashType, size);
}