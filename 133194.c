/* extern */ x3f_return_t x3f_delete(x3f_t *x3f)
{
  x3f_directory_section_t *DS;
  int d;

  if (x3f == NULL)
    return X3F_ARGUMENT_ERROR;

  DS = &x3f->directory_section;
  if (DS->num_directory_entries > 50)
    return X3F_ARGUMENT_ERROR;

  for (d = 0; d < DS->num_directory_entries; d++)
  {
    x3f_directory_entry_t *DE = &DS->directory_entry[d];
    x3f_directory_entry_header_t *DEH = &DE->header;
    if (DEH->identifier == X3F_SECp)
    {
      x3f_property_list_t *PL = &DEH->data_subsection.property_list;
      if (PL)
      {
        int i;
      }
      FREE(PL->property_table.element);
      FREE(PL->data);
    }

    if (DEH->identifier == X3F_SECi)
    {
      x3f_image_data_t *ID = &DEH->data_subsection.image_data;

      if (ID)
      {
        cleanup_huffman(&ID->huffman);
        cleanup_true(&ID->tru);
        cleanup_quattro(&ID->quattro);
        FREE(ID->data);
      }
    }

    if (DEH->identifier == X3F_SECc)
    {
      x3f_camf_t *CAMF = &DEH->data_subsection.camf;
      int i;
      if (CAMF)
      {
        FREE(CAMF->data);
        FREE(CAMF->table.element);
        cleanup_huffman_tree(&CAMF->tree);
        FREE(CAMF->decoded_data);
        for (i = 0; i < CAMF->entry_table.size; i++)
        {
          free_camf_entry(&CAMF->entry_table.element[i]);
        }
      }
      FREE(CAMF->entry_table.element);
    }
  }

  FREE(DS->directory_entry);
  FREE(x3f);

  return X3F_OK;
}