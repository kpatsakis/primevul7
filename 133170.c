static void x3f_load_huffman_compressed(x3f_info_t *I,
                                        x3f_directory_entry_t *DE, int bits,
                                        int use_map_table)
{
  x3f_directory_entry_header_t *DEH = &DE->header;
  x3f_image_data_t *ID = &DEH->data_subsection.image_data;
  x3f_huffman_t *HUF = ID->huffman;
  int table_size = 1 << bits;
  int row_offsets_size = ID->rows * sizeof(HUF->row_offsets.element[0]);

  GET_TABLE(HUF->table, GET4, table_size, uint32_t);

  if (!ID->data_size)
    ID->data_size = read_data_block(&ID->data, I, DE, row_offsets_size);

  GET_TABLE(HUF->row_offsets, GET4, ID->rows, uint32_t);

  new_huffman_tree(&HUF->tree, bits);
  populate_huffman_tree(&HUF->tree, &HUF->table, &HUF->mapping);

  huffman_decode(I, DE, bits);
}