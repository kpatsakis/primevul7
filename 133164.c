static void x3f_load_true(x3f_info_t *I, x3f_directory_entry_t *DE)
{
  x3f_directory_entry_header_t *DEH = &DE->header;
  x3f_image_data_t *ID = &DEH->data_subsection.image_data;
  x3f_true_t *TRU = new_true(&ID->tru);
  x3f_quattro_t *Q = NULL;
  int i;

  if (ID->type_format == X3F_IMAGE_RAW_QUATTRO ||
      ID->type_format == X3F_IMAGE_RAW_SDQ ||
      ID->type_format == X3F_IMAGE_RAW_SDQH ||
      ID->type_format == X3F_IMAGE_RAW_SDQH2)
  {
    Q = new_quattro(&ID->quattro);

    for (i = 0; i < TRUE_PLANES; i++)
    {
      GET2(Q->plane[i].columns);
      GET2(Q->plane[i].rows);
    }

    if (Q->plane[0].rows == ID->rows / 2)
    {
      Q->quattro_layout = 1;
    }
    else if (Q->plane[0].rows == ID->rows)
    {
      Q->quattro_layout = 0;
    }
    else
    {
      throw LIBRAW_EXCEPTION_IO_CORRUPT;
    }
  }

  /* Read TRUE header data */
  GET2(TRU->seed[0]);
  GET2(TRU->seed[1]);
  GET2(TRU->seed[2]);
  GET2(TRU->unknown);
  GET_TRUE_HUFF_TABLE(TRU->table);

  if (ID->type_format == X3F_IMAGE_RAW_QUATTRO ||
      ID->type_format == X3F_IMAGE_RAW_SDQ ||
      ID->type_format == X3F_IMAGE_RAW_SDQH ||
      ID->type_format == X3F_IMAGE_RAW_SDQH2)
  {
    GET4(Q->unknown);
  }

  GET_TABLE(TRU->plane_size, GET4, TRUE_PLANES, uint32_t);

  /* Read image data */
  if (!ID->data_size)
    ID->data_size = read_data_block(&ID->data, I, DE, 0);

  /* TODO: can it be fewer than 8 bits? Maybe taken from TRU->table? */
  new_huffman_tree(&TRU->tree, 8);

  populate_true_huffman_tree(&TRU->tree, &TRU->table);

#ifdef DBG_PRNT
  print_huffman_tree(TRU->tree.nodes, 0, 0);
#endif

  TRU->plane_address[0] = (uint8_t *)ID->data;
  for (i = 1; i < TRUE_PLANES; i++)
    TRU->plane_address[i] = TRU->plane_address[i - 1] +
                            (((TRU->plane_size.element[i - 1] + 15) / 16) * 16);

  if ((ID->type_format == X3F_IMAGE_RAW_QUATTRO ||
       ID->type_format == X3F_IMAGE_RAW_SDQ ||
       ID->type_format == X3F_IMAGE_RAW_SDQH ||
       ID->type_format == X3F_IMAGE_RAW_SDQH2) &&
      Q->quattro_layout)
  {
    uint32_t columns = Q->plane[0].columns;
    uint32_t rows = Q->plane[0].rows;
    uint32_t channels = 3;
    uint32_t size = columns * rows * channels;

    TRU->x3rgb16.columns = columns;
    TRU->x3rgb16.rows = rows;
    TRU->x3rgb16.channels = channels;
    TRU->x3rgb16.row_stride = columns * channels;
    TRU->x3rgb16.buf = malloc(sizeof(uint16_t) * size);
    TRU->x3rgb16.data = (uint16_t *)TRU->x3rgb16.buf;

    columns = Q->plane[2].columns;
    rows = Q->plane[2].rows;
    channels = 1;
    size = columns * rows * channels;

    Q->top16.columns = columns;
    Q->top16.rows = rows;
    Q->top16.channels = channels;
    Q->top16.row_stride = columns * channels;
    Q->top16.buf = malloc(sizeof(uint16_t) * size);
    Q->top16.data = (uint16_t *)Q->top16.buf;
  }
  else
  {
    uint32_t size = ID->columns * ID->rows * 3;

    TRU->x3rgb16.columns = ID->columns;
    TRU->x3rgb16.rows = ID->rows;
    TRU->x3rgb16.channels = 3;
    TRU->x3rgb16.row_stride = ID->columns * 3;
    TRU->x3rgb16.buf = malloc(sizeof(uint16_t) * size);
    TRU->x3rgb16.data = (uint16_t *)TRU->x3rgb16.buf;
  }

  true_decode(I, DE);
}