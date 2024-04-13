static void print_huffman_tree(x3f_huffnode_t *t, int length, uint32_t code)
{
  char buf1[100];
  char buf2[100];

  x3f_printf(DEBUG, "%*s (%s,%s) %s (%s)\n", length,
             length < 1 ? "-" : (code & 1) ? "1" : "0",
             t->branch[0] == NULL ? "-" : "0", t->branch[1] == NULL ? "-" : "1",
             t->leaf == UNDEFINED_LEAF ? "-"
                                       : (sprintf(buf1, "%x", t->leaf), buf1),
             display_code(length, code, buf2));

  code = code << 1;
  if (t->branch[0])
    print_huffman_tree(t->branch[0], length + 1, code + 0);
  if (t->branch[1])
    print_huffman_tree(t->branch[1], length + 1, code + 1);
}