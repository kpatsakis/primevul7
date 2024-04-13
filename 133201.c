static int32_t get_true_diff(bit_state_t *BS, x3f_hufftree_t *HTP)
{
  int32_t diff;
  x3f_huffnode_t *node = &HTP->nodes[0];
  uint8_t bits;

  while (node->branch[0] != NULL || node->branch[1] != NULL)
  {
    uint8_t bit = get_bit(BS);
    x3f_huffnode_t *new_node = node->branch[bit];

    node = new_node;
    if (node == NULL)
    {
      /* TODO: Shouldn't this be treated as a fatal error? */
      return 0;
    }
  }

  bits = node->leaf;

  if (bits == 0)
    diff = 0;
  else
  {
    uint8_t first_bit = get_bit(BS);
    int i;

    diff = first_bit;

    for (i = 1; i < bits; i++)
      diff = (diff << 1) + get_bit(BS);

    if (first_bit == 0)
      diff -= (1 << bits) - 1;
  }

  return diff;
}