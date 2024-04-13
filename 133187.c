static void camf_decode_type5(x3f_camf_t *CAMF)
{
  int32_t acc = CAMF->t5.decode_bias;

  uint8_t *dst;

  x3f_hufftree_t *tree = &CAMF->tree;
  bit_state_t BS;

  int32_t i;

  CAMF->decoded_data_size = CAMF->t5.decoded_data_size;
  CAMF->decoded_data = malloc(CAMF->decoded_data_size);

  dst = (uint8_t *)CAMF->decoded_data;

  set_bit_state(&BS, CAMF->decoding_start);

  for (i = 0; i < CAMF->decoded_data_size; i++)
  {
    int32_t diff = get_true_diff(&BS, tree);

    acc = acc + diff;
    *dst++ = (uint8_t)(acc & 0xff);
  }
}