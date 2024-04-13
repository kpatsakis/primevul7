cons_node_free_alone(Node* node)
{
  NODE_CAR(node) = 0;
  NODE_CDR(node) = 0;
  onig_node_free(node);
}