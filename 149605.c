divide_look_behind_alternatives(Node* node)
{
  Node *head, *np, *insert_node;
  AnchorNode* an = NANCHOR(node);
  int anc_type = an->type;

  head = an->target;
  np = NCAR(head);
  swap_node(node, head);
  NCAR(node) = head;
  NANCHOR(head)->target = np;

  np = node;
  while ((np = NCDR(np)) != NULL_NODE) {
    insert_node = onig_node_new_anchor(anc_type);
    CHECK_NULL_RETURN_MEMERR(insert_node);
    NANCHOR(insert_node)->target = NCAR(np);
    NCAR(np) = insert_node;
  }

  if (anc_type == ANCHOR_LOOK_BEHIND_NOT) {
    np = node;
    do {
      SET_NTYPE(np, NT_LIST);  /* alt -> list */
    } while ((np = NCDR(np)) != NULL_NODE);
  }
  return 0;
}