subexp_inf_recursive_check_trav(Node* node, ScanEnv* env)
{
  int type;
  int r = 0;

  type = NTYPE(node);
  switch (type) {
  case NT_LIST:
  case NT_ALT:
    do {
      r = subexp_inf_recursive_check_trav(NCAR(node), env);
    } while (r == 0 && IS_NOT_NULL(node = NCDR(node)));
    break;

  case NT_QTFR:
    r = subexp_inf_recursive_check_trav(NQTFR(node)->target, env);
    break;

  case NT_ANCHOR:
    {
      AnchorNode* an = NANCHOR(node);
      switch (an->type) {
      case ANCHOR_PREC_READ:
      case ANCHOR_PREC_READ_NOT:
      case ANCHOR_LOOK_BEHIND:
      case ANCHOR_LOOK_BEHIND_NOT:
	r = subexp_inf_recursive_check_trav(an->target, env);
	break;
      }
    }
    break;

  case NT_ENCLOSE:
    {
      EncloseNode* en = NENCLOSE(node);

      if (IS_ENCLOSE_RECURSION(en)) {
	SET_ENCLOSE_STATUS(node, NST_MARK1);
	r = subexp_inf_recursive_check(en->target, env, 1);
	if (r > 0) return ONIGERR_NEVER_ENDING_RECURSION;
	CLEAR_ENCLOSE_STATUS(node, NST_MARK1);
      }
      r = subexp_inf_recursive_check_trav(en->target, env);
    }

    break;

  default:
    break;
  }

  return r;
}