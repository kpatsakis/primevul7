static x3f_directory_entry_t *x3f_get(x3f_t *x3f, uint32_t type,
                                      uint32_t image_type)
{
  x3f_directory_section_t *DS;
  int d;

  if (x3f == NULL)
    return NULL;

  DS = &x3f->directory_section;

  for (d = 0; d < DS->num_directory_entries; d++)
  {
    x3f_directory_entry_t *DE = &DS->directory_entry[d];
    x3f_directory_entry_header_t *DEH = &DE->header;

    if (DEH->identifier == type)
    {
      switch (DEH->identifier)
      {
      case X3F_SECi:
      {
        x3f_image_data_t *ID = &DEH->data_subsection.image_data;

        if (ID->type_format == image_type)
          return DE;
      }
      break;
      default:
        return DE;
      }
    }
  }

  return NULL;
}