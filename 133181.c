static void camf_decode_type4(x3f_camf_t *CAMF)
{
  uint32_t seed = CAMF->t4.decode_bias;
  int row;

  uint8_t *dst;
  uint32_t dst_size = CAMF->t4.decoded_data_size;
  uint8_t *dst_end;

  bool_t odd_dst = 0;

  x3f_hufftree_t *tree = &CAMF->tree;
  bit_state_t BS;

  int32_t row_start_acc[2][2];
  uint32_t rows = CAMF->t4.block_count;
  uint32_t cols = CAMF->t4.block_size;

  CAMF->decoded_data_size = dst_size;

  CAMF->decoded_data = malloc(CAMF->decoded_data_size);
  memset(CAMF->decoded_data, 0, CAMF->decoded_data_size);

  dst = (uint8_t *)CAMF->decoded_data;
  dst_end = dst + dst_size;

  set_bit_state(&BS, CAMF->decoding_start);

  row_start_acc[0][0] = seed;
  row_start_acc[0][1] = seed;
  row_start_acc[1][0] = seed;
  row_start_acc[1][1] = seed;

  for (row = 0; row < rows; row++)
  {
    int col;
    bool_t odd_row = row & 1;
    int32_t acc[2];

    /* We loop through all the columns and the rows. But the actual
       data is smaller than that, so we break the loop when reaching
       the end. */
    for (col = 0; col < cols; col++)
    {
      bool_t odd_col = col & 1;
      int32_t diff = get_true_diff(&BS, tree);
      int32_t prev = col < 2 ? row_start_acc[odd_row][odd_col] : acc[odd_col];
      int32_t value = prev + diff;

      acc[odd_col] = value;
      if (col < 2)
        row_start_acc[odd_row][odd_col] = value;

      switch (odd_dst)
      {
      case 0:
        *dst++ = (uint8_t)((value >> 4) & 0xff);

        if (dst >= dst_end)
        {
          goto ready;
        }

        *dst = (uint8_t)((value << 4) & 0xf0);
        break;
      case 1:
        *dst++ |= (uint8_t)((value >> 8) & 0x0f);

        if (dst >= dst_end)
        {
          goto ready;
        }

        *dst++ = (uint8_t)((value << 0) & 0xff);

        if (dst >= dst_end)
        {
          goto ready;
        }

        break;
      }

      odd_dst = !odd_dst;
    } /* end col */
  }   /* end row */

ready:;
}