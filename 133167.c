static void x3f_load_property_list(x3f_info_t *I, x3f_directory_entry_t *DE)
{
  x3f_directory_entry_header_t *DEH = &DE->header;
  x3f_property_list_t *PL = &DEH->data_subsection.property_list;
  int i;

  read_data_set_offset(I, DE, X3F_PROPERTY_LIST_HEADER_SIZE);

  GET_PROPERTY_TABLE(PL->property_table, PL->num_properties);

  if (!PL->data_size)
    PL->data_size = read_data_block(&PL->data, I, DE, 0);
  uint32_t maxoffset = PL->data_size / sizeof(utf16_t) -
                       2; // at least 2 chars, value + terminating 0x0000

  for (i = 0; i < PL->num_properties; i++)
  {
    x3f_property_t *P = &PL->property_table.element[i];
    if (P->name_offset > maxoffset || P->value_offset > maxoffset)
      throw LIBRAW_EXCEPTION_IO_CORRUPT;
    P->name = ((utf16_t *)PL->data + P->name_offset);
    P->value = ((utf16_t *)PL->data + P->value_offset);
  }
}