subexp_recursive_check_trav(Node* node, ScanEnv* env)
{
# define FOUND_CALLED_NODE    1

  int type;
  int r = 0;

  type = NTYPE(node);
  switch (type) {
  case NT_LIST:
  case NT_ALT:
    {
      int ret;
      do {
	ret = subexp_recursive_check_trav(NCAR(node), env);
	if (ret == FOUND_CALLED_NODE) r = FOUND_CALLED_NODE;
	else if (ret < 0) return ret;
      } while (IS_NOT_NULL(node = NCDR(node)));
    }
    break;

  case NT_QTFR:
    r = subexp_recursive_check_trav(NQTFR(node)->target, env);
    if (NQTFR(node)->upper == 0) {
      if (r == FOUND_CALLED_NODE)
	NQTFR(node)->is_refered = 1;
    }
    break;

  case NT_ANCHOR:
    {
      AnchorNode* an = NANCHOR(node);
      switch (an->type) {
      case ANCHOR_PREC_READ:
      case ANCHOR_PREC_READ_NOT:
      case ANCHOR_LOOK_BEHIND:
      case ANCHOR_LOOK_BEHIND_NOT:
	r = subexp_recursive_check_trav(an->target, env);
	break;
      }
    }
    break;

  case NT_ENCLOSE:
    {
      EncloseNode* en = NENCLOSE(node);

      if (! IS_ENCLOSE_RECURSION(en)) {
	if (IS_ENCLOSE_CALLED(en)) {
	  SET_ENCLOSE_STATUS(node, NST_MARK1);
	  r = subexp_recursive_check(en->target);
	  if (r != 0) SET_ENCLOSE_STATUS(node, NST_RECURSION);
	  CLEAR_ENCLOSE_STATUS(node, NST_MARK1);
	}
      }
      r = subexp_recursive_check_trav(en->target, env);
      if (IS_ENCLOSE_CALLED(en))
	r |= FOUND_CALLED_NODE;
    }
    break;

  default:
    break;
  }

  return r;
}