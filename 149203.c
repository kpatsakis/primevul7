make_list_or_alt(NodeType type, int n, Node* ns[])
{
  Node* r;

  if (n <= 0) return NULL_NODE;

  if (n == 1) {
    r = node_new();
    CHECK_NULL_RETURN(r);
    NODE_SET_TYPE(r, type);
    NODE_CAR(r) = ns[0];
    NODE_CDR(r) = NULL_NODE;
  }
  else {
    Node* right;

    r = node_new();
    CHECK_NULL_RETURN(r);

    right = make_list_or_alt(type, n - 1, ns + 1);
    if (IS_NULL(right)) {
      onig_node_free(r);
      return NULL_NODE;
    }

    NODE_SET_TYPE(r, type);
    NODE_CAR(r) = ns[0];
    NODE_CDR(r) = right;
  }

  return r;
}