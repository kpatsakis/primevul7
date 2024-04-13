node_new_anychar_with_fixed_option(OnigOptionType option)
{
  CtypeNode* ct;
  Node* node;

  node = node_new_anychar();
  ct = CTYPE_(node);
  ct->options = option;
  NODE_STATUS_ADD(node, NST_FIXED_OPTION);
  return node;
}