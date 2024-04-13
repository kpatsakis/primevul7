onig_node_new_anchor(int type, int ascii_mode)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_ANCHOR);
  ANCHOR_(node)->type       = type;
  ANCHOR_(node)->char_len   = -1;
  ANCHOR_(node)->ascii_mode = ascii_mode;
  return node;
}