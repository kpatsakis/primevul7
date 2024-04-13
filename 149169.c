node_new_ctype(int type, int not, OnigOptionType options)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_CTYPE);
  CTYPE_(node)->ctype   = type;
  CTYPE_(node)->not     = not;
  CTYPE_(node)->options = options;
  CTYPE_(node)->ascii_mode = IS_ASCII_MODE_CTYPE_OPTION(type, options);
  return node;
}