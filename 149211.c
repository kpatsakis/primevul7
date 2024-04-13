node_new_enclosure_if_else(Node* cond, Node* Then, Node* Else)
{
  Node* n;
  n = node_new_enclosure(ENCLOSURE_IF_ELSE);
  CHECK_NULL_RETURN(n);

  NODE_BODY(n) = cond;
  ENCLOSURE_(n)->te.Then = Then;
  ENCLOSURE_(n)->te.Else = Else;
  return n;
}