list_node_free_not_car(Node* node)
{
  Node* next_node;

 start:
  if (IS_NULL(node)) return;

  next_node = NODE_CDR(node);
  xfree(node);
  node = next_node;
  goto start;
}