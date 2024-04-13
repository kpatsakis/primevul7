sort_version_list(smartlist_t *versions, int remove_duplicates)
{
  smartlist_sort(versions, compare_tor_version_str_ptr_);

  if (remove_duplicates)
    smartlist_uniq(versions, compare_tor_version_str_ptr_, tor_free_);
}