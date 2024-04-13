static int32_t x3f_load_image_verbatim_size(x3f_info_t *I,
                                            x3f_directory_entry_t *DE)
{
  x3f_directory_entry_header_t *DEH = &DE->header;
  x3f_image_data_t *ID = &DEH->data_subsection.image_data;
  return data_block_size(&ID->data, I, DE, 0);
}