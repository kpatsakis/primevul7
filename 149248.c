node_new_list(Node* left, Node* right)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_LIST);
  NODE_CAR(node)  = left;
  NODE_CDR(node) = right;
  return node;
}