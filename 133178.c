static void populate_true_huffman_tree(x3f_hufftree_t *tree,
                                       x3f_true_huffman_t *table)
{
  int i;

  new_node(tree);

  for (i = 0; i < table->size; i++)
  {
    x3f_true_huffman_element_t *element = &table->element[i];
    uint32_t length = element->code_size;

    if (length != 0)
    {
      /* add_code_to_tree wants the code right adjusted */
      uint32_t code = ((element->code) >> (8 - length)) & 0xff;
      uint32_t value = i;

      add_code_to_tree(tree, length, code, value);

#ifdef DBG_PRNT
      {
        char buffer[100];

        x3f_printf(DEBUG, "H %5d : %5x : %5d : %02x %08x (%08x) (%s)\n", i, i,
                   value, length, code, value,
                   display_code(length, code, buffer));
      }
#endif
    }
  }
}