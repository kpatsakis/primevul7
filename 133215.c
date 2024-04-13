static void x3f_setup_camf_entries(x3f_camf_t *CAMF)
{
  uint8_t *p = (uint8_t *)CAMF->decoded_data;
  uint8_t *end = p + CAMF->decoded_data_size;
  camf_entry_t *entry = NULL;
  int i;

  for (i = 0; p < end; i++)
  {
    uint32_t *p4 = (uint32_t *)p;

    switch (*p4)
    {
    case X3F_CMbP:
    case X3F_CMbT:
    case X3F_CMbM:
      break;
    default:
      goto stop;
    }

    /* TODO: lots of realloc - may be inefficient */
    entry = (camf_entry_t *)realloc(entry, (i + 1) * sizeof(camf_entry_t));

    /* Pointer */
    entry[i].entry = p;

    /* Header */
    entry[i].id = *p4++;
    entry[i].version = *p4++;
    entry[i].entry_size = *p4++;
    entry[i].name_offset = *p4++;
    entry[i].value_offset = *p4++;

    /* Compute adresses and sizes */
    entry[i].name_address = (char *)(p + entry[i].name_offset);
    entry[i].value_address = p + entry[i].value_offset;
    entry[i].name_size = entry[i].value_offset - entry[i].name_offset;
    entry[i].value_size = entry[i].entry_size - entry[i].value_offset;

    entry[i].text_size = 0;
    entry[i].text = NULL;
    entry[i].property_num = 0;
    entry[i].property_name = NULL;
    entry[i].property_value = NULL;
    entry[i].matrix_type = 0;
    entry[i].matrix_dim = 0;
    entry[i].matrix_data_off = 0;
    entry[i].matrix_data = NULL;
    entry[i].matrix_dim_entry = NULL;

    entry[i].matrix_decoded = NULL;

    switch (entry[i].id)
    {
    case X3F_CMbP:
      x3f_setup_camf_property_entry(&entry[i]);
      break;
    case X3F_CMbT:
      x3f_setup_camf_text_entry(&entry[i]);
      break;
    case X3F_CMbM:
      x3f_setup_camf_matrix_entry(&entry[i]);
      break;
    }

    p += entry[i].entry_size;
  }

stop:

  CAMF->entry_table.size = i;
  CAMF->entry_table.element = entry;
}