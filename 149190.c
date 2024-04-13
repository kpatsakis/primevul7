node_new_cclass(void)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_CCLASS);
  initialize_cclass(CCLASS_(node));
  return node;
}