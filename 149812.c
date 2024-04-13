check_type_tree(Node* node, int type_mask, int enclose_mask, int anchor_mask)
{
  int type, r = 0;

  type = NTYPE(node);
  if ((NTYPE2BIT(type) & type_mask) == 0)
    return 1;

  switch (type) {
  case NT_LIST:
  case NT_ALT:
    do {
      r = check_type_tree(NCAR(node), type_mask, enclose_mask,
			  anchor_mask);
    } while (r == 0 && IS_NOT_NULL(node = NCDR(node)));
    break;

  case NT_QTFR:
    r = check_type_tree(NQTFR(node)->target, type_mask, enclose_mask,
			anchor_mask);
    break;

  case NT_ENCLOSE:
    {
      EncloseNode* en = NENCLOSE(node);
      if ((en->type & enclose_mask) == 0)
	return 1;

      r = check_type_tree(en->target, type_mask, enclose_mask, anchor_mask);
    }
    break;

  case NT_ANCHOR:
    type = NANCHOR(node)->type;
    if ((type & anchor_mask) == 0)
      return 1;

    if (NANCHOR(node)->target)
      r = check_type_tree(NANCHOR(node)->target,
			  type_mask, enclose_mask, anchor_mask);
    break;

  default:
    break;
  }
  return r;
}