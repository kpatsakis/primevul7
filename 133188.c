static void x3f_setup_camf_matrix_entry(camf_entry_t *entry)
{
  int i;
  int totalsize = 1;

  uint8_t *e = (uint8_t *)entry->entry;
  uint8_t *v = (uint8_t *)entry->value_address;
  uint32_t type = entry->matrix_type = *(uint32_t *)(v + 0);
  uint32_t dim = entry->matrix_dim = *(uint32_t *)(v + 4);
  uint32_t off = entry->matrix_data_off = *(uint32_t *)(v + 8);
  camf_dim_entry_t *dentry = entry->matrix_dim_entry =
      (camf_dim_entry_t *)malloc(dim * sizeof(camf_dim_entry_t));

  for (i = 0; i < dim; i++)
  {
    uint32_t size = dentry[i].size = *(uint32_t *)(v + 12 + 12 * i + 0);
    dentry[i].name_offset = *(uint32_t *)(v + 12 + 12 * i + 4);
    dentry[i].n = *(uint32_t *)(v + 12 + 12 * i + 8);
    dentry[i].name = (char *)(e + dentry[i].name_offset);

    if (dentry[i].n != i)
    {
    }

    totalsize *= size;
  }

  set_matrix_element_info(type, &entry->matrix_element_size,
                          &entry->matrix_decoded_type);
  entry->matrix_data = (void *)(e + off);

  entry->matrix_elements = totalsize;
  entry->matrix_used_space = entry->entry_size - off;

  /* This estimate only works for matrices above a certain size */
  entry->matrix_estimated_element_size = entry->matrix_used_space / totalsize;

  get_matrix_copy(entry);
}