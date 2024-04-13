node_new_anychar(void)
{
  Node* node = node_new_ctype(CTYPE_ANYCHAR, 0, ONIG_OPTION_NONE);
  return node;
}