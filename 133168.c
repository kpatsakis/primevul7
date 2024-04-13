static void true_decode_one_color(x3f_image_data_t *ID, int color)
{
  x3f_true_t *TRU = ID->tru;
  x3f_quattro_t *Q = ID->quattro;
  uint32_t seed = TRU->seed[color]; /* TODO : Is this correct ? */
  int row;

  x3f_hufftree_t *tree = &TRU->tree;
  bit_state_t BS;

  int32_t row_start_acc[2][2];
  uint32_t rows = ID->rows;
  uint32_t cols = ID->columns;
  x3f_area16_t *area = &TRU->x3rgb16;
  uint16_t *dst = area->data + color;

  set_bit_state(&BS, TRU->plane_address[color]);

  row_start_acc[0][0] = seed;
  row_start_acc[0][1] = seed;
  row_start_acc[1][0] = seed;
  row_start_acc[1][1] = seed;

  if (ID->type_format == X3F_IMAGE_RAW_QUATTRO ||
      ID->type_format == X3F_IMAGE_RAW_SDQ ||
      ID->type_format == X3F_IMAGE_RAW_SDQH ||
      ID->type_format == X3F_IMAGE_RAW_SDQH2)
  {
    rows = Q->plane[color].rows;
    cols = Q->plane[color].columns;

    if (Q->quattro_layout && color == 2)
    {
      area = &Q->top16;
      dst = area->data;
    }
  }
  else
  {
  }

  if (rows != area->rows || cols < area->columns)
    throw LIBRAW_EXCEPTION_IO_CORRUPT;

  for (row = 0; row < rows; row++)
  {
    int col;
    bool_t odd_row = row & 1;
    int32_t acc[2];

    for (col = 0; col < cols; col++)
    {
      bool_t odd_col = col & 1;
      int32_t diff = get_true_diff(&BS, tree);
      int32_t prev = col < 2 ? row_start_acc[odd_row][odd_col] : acc[odd_col];
      int32_t value = prev + diff;

      acc[odd_col] = value;
      if (col < 2)
        row_start_acc[odd_row][odd_col] = value;

      /* Discard additional data at the right for binned Quattro plane 2 */
      if (col >= area->columns)
        continue;

      *dst = value;
      dst += area->channels;
    }
  }
}