static void x3f_load_camf_decode_type4(x3f_camf_t *CAMF)
{
  int i;
  uint8_t *p;
  x3f_true_huffman_element_t *element = NULL;

  for (i = 0, p = (uint8_t *)CAMF->data; *p != 0; i++)
  {
    /* TODO: Is this too expensive ??*/
    element = (x3f_true_huffman_element_t *)realloc(element,
                                                    (i + 1) * sizeof(*element));

    element[i].code_size = *p++;
    element[i].code = *p++;
  }

  CAMF->table.size = i;
  CAMF->table.element = element;

  /* TODO: where does the values 28 and 32 come from? */
#define CAMF_T4_DATA_SIZE_OFFSET 28
#define CAMF_T4_DATA_OFFSET 32
  CAMF->decoding_size =
      *(uint32_t *)((unsigned char *)CAMF->data + CAMF_T4_DATA_SIZE_OFFSET);
  CAMF->decoding_start = (uint8_t *)CAMF->data + CAMF_T4_DATA_OFFSET;

  /* TODO: can it be fewer than 8 bits? Maybe taken from TRU->table? */
  new_huffman_tree(&CAMF->tree, 8);

  populate_true_huffman_tree(&CAMF->tree, &CAMF->table);

#ifdef DBG_PRNT
  print_huffman_tree(CAMF->tree.nodes, 0, 0);
#endif

  camf_decode_type4(CAMF);
}