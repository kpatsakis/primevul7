static void add_code_to_tree(x3f_hufftree_t *tree, int length, uint32_t code,
                             uint32_t value)
{
  int i;

  x3f_huffnode_t *t = tree->nodes;

  for (i = 0; i < length; i++)
  {
    int pos = PATTERN_BIT_POS(length, i);
    int bit = (code >> pos) & 1;
    x3f_huffnode_t *t_next = t->branch[bit];

    if (t_next == NULL)
      t_next = t->branch[bit] = new_node(tree);

    t = t_next;
  }

  t->leaf = value;
}