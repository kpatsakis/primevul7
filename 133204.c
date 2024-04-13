static int32_t get_huffman_diff(bit_state_t *BS, x3f_hufftree_t *HTP)
{
  int32_t diff;
  x3f_huffnode_t *node = &HTP->nodes[0];

  while (node->branch[0] != NULL || node->branch[1] != NULL)
  {
    uint8_t bit = get_bit(BS);
    x3f_huffnode_t *new_node = node->branch[bit];

    node = new_node;
    if (node == NULL)
    {
      /* TODO: Shouldn't this be treated as a fatal error? */
      throw LIBRAW_EXCEPTION_IO_CORRUPT;
      return 0;
    }
  }

  diff = node->leaf;

  return diff;
}