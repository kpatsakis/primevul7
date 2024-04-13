node_new(void)
{
  Node* node;

  node = (Node* )xmalloc(sizeof(Node));
  /* xmemset(node, 0, sizeof(Node)); */
  return node;
}