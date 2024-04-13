static void populate_huffman_tree(x3f_hufftree_t *tree, x3f_table32_t *table,
                                  x3f_table16_t *mapping)
{
  int i;

  new_node(tree);

  for (i = 0; i < table->size; i++)
  {
    uint32_t element = table->element[i];

    if (element != 0)
    {
      uint32_t length = HUF_TREE_GET_LENGTH(element);
      uint32_t code = HUF_TREE_GET_CODE(element);
      uint32_t value;

      /* If we have a valid mapping table - then the value from the
         mapping table shall be used. Otherwise we use the current
         index in the table as value. */
      if (table->size == mapping->size)
        value = mapping->element[i];
      else
        value = i;

      add_code_to_tree(tree, length, code, value);

#ifdef DBG_PRNT
      {
        char buffer[100];

        x3f_printf(DEBUG, "H %5d : %5x : %5d : %02x %08x (%08x) (%s)\n", i, i,
                   value, length, code, element,
                   display_code(length, code, buffer));
      }
#endif
    }
  }
}