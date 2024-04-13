subexp_recursive_check(Node* node)
{
  int r = 0;

  switch (NTYPE(node)) {
  case NT_LIST:
  case NT_ALT:
    do {
      r |= subexp_recursive_check(NCAR(node));
    } while (IS_NOT_NULL(node = NCDR(node)));
    break;

  case NT_QTFR:
    r = subexp_recursive_check(NQTFR(node)->target);
    break;

  case NT_ANCHOR:
    {
      AnchorNode* an = NANCHOR(node);
      switch (an->type) {
      case ANCHOR_PREC_READ:
      case ANCHOR_PREC_READ_NOT:
      case ANCHOR_LOOK_BEHIND:
      case ANCHOR_LOOK_BEHIND_NOT:
	r = subexp_recursive_check(an->target);
	break;
      }
    }
    break;

  case NT_CALL:
    r = subexp_recursive_check(NCALL(node)->target);
    if (r != 0) SET_CALL_RECURSION(node);
    break;

  case NT_ENCLOSE:
    if (IS_ENCLOSE_MARK2(NENCLOSE(node)))
      return 0;
    else if (IS_ENCLOSE_MARK1(NENCLOSE(node)))
      return 1; /* recursion */
    else {
      SET_ENCLOSE_STATUS(node, NST_MARK2);
      r = subexp_recursive_check(NENCLOSE(node)->target);
      CLEAR_ENCLOSE_STATUS(node, NST_MARK2);
    }
    break;

  default:
    break;
  }

  return r;
}