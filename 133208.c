static void x3f_load_huffman(x3f_info_t *I, x3f_directory_entry_t *DE, int bits,
                             int use_map_table, int row_stride)
{
  x3f_directory_entry_header_t *DEH = &DE->header;
  x3f_image_data_t *ID = &DEH->data_subsection.image_data;
  x3f_huffman_t *HUF = new_huffman(&ID->huffman);
  uint32_t size;

  if (use_map_table)
  {
    int table_size = 1 << bits;

    GET_TABLE(HUF->mapping, GET2, table_size, uint16_t);
  }

  switch (ID->type_format)
  {
  case X3F_IMAGE_RAW_HUFFMAN_X530:
  case X3F_IMAGE_RAW_HUFFMAN_10BIT:
    size = ID->columns * ID->rows * 3;
    HUF->x3rgb16.columns = ID->columns;
    HUF->x3rgb16.rows = ID->rows;
    HUF->x3rgb16.channels = 3;
    HUF->x3rgb16.row_stride = ID->columns * 3;
    HUF->x3rgb16.buf = malloc(sizeof(uint16_t) * size);
    HUF->x3rgb16.data = (uint16_t *)HUF->x3rgb16.buf;
    break;
  case X3F_IMAGE_THUMB_HUFFMAN:
    size = ID->columns * ID->rows * 3;
    HUF->rgb8.columns = ID->columns;
    HUF->rgb8.rows = ID->rows;
    HUF->rgb8.channels = 3;
    HUF->rgb8.row_stride = ID->columns * 3;
    HUF->rgb8.buf = malloc(sizeof(uint8_t) * size);
    HUF->rgb8.data = (uint8_t *)HUF->rgb8.buf;
    break;
  default:
    /* TODO: Shouldn't this be treated as a fatal error? */
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
  }

  if (row_stride == 0)
    return x3f_load_huffman_compressed(I, DE, bits, use_map_table);
  else
    return x3f_load_huffman_not_compressed(I, DE, bits, use_map_table,
                                           row_stride);
}