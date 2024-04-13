compile_length_option_node(EncloseNode* node, regex_t* reg)
{
  int tlen;
  OnigOptionType prev = reg->options;

  reg->options = node->option;
  tlen = compile_length_tree(node->target, reg);
  reg->options = prev;

  if (tlen < 0) return tlen;

  if (IS_DYNAMIC_OPTION(prev ^ node->option)) {
    return SIZE_OP_SET_OPTION_PUSH + SIZE_OP_SET_OPTION + SIZE_OP_FAIL
           + tlen + SIZE_OP_SET_OPTION;
  }
  else
    return tlen;
}