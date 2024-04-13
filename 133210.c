static void huffman_decode(x3f_info_t *I, x3f_directory_entry_t *DE, int bits)
{
  x3f_directory_entry_header_t *DEH = &DE->header;
  x3f_image_data_t *ID = &DEH->data_subsection.image_data;

  int row;
  int minimum = 0;
  int offset = legacy_offset;

  for (row = 0; row < ID->rows; row++)
    huffman_decode_row(I, DE, bits, row, offset, &minimum);

  if (auto_legacy_offset && minimum < 0)
  {
    offset = -minimum;
    for (row = 0; row < ID->rows; row++)
      huffman_decode_row(I, DE, bits, row, offset, &minimum);
  }
}