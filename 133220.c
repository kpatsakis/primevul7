static void free_camf_entry(camf_entry_t *entry)
{
  FREE(entry->property_name);
  FREE(entry->property_value);
  FREE(entry->matrix_decoded);
  FREE(entry->matrix_dim_entry);
}