node_new_true_anychar(Node** node, ScanEnv* env)
{
  Node* n;

  n = node_new_anychar_with_fixed_option(ONIG_OPTION_MULTILINE);
  CHECK_NULL_RETURN_MEMERR(n);
  *node = n;
  return 0;
}