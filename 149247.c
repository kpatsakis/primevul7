node_new_no_newline(Node** node, ScanEnv* env)
{
  Node* n;

  n = node_new_anychar_with_fixed_option(ONIG_OPTION_NONE);
  CHECK_NULL_RETURN_MEMERR(n);
  *node = n;
  return 0;
}