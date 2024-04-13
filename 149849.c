subexp_inf_recursive_check(Node* node, ScanEnv* env, int head)
{
  int type;
  int r = 0;

  type = NTYPE(node);
  switch (type) {
  case NT_LIST:
    {
      Node *x;
      OnigDistance min;
      int ret;

      x = node;
      do {
	ret = subexp_inf_recursive_check(NCAR(x), env, head);
	if (ret < 0 || ret == RECURSION_INFINITE) return ret;
	r |= ret;
	if (head) {
	  ret = get_min_match_length(NCAR(x), &min, env);
	  if (ret != 0) return ret;
	  if (min != 0) head = 0;
	}
      } while (IS_NOT_NULL(x = NCDR(x)));
    }
    break;

  case NT_ALT:
    {
      int ret;
      r = RECURSION_EXIST;
      do {
	ret = subexp_inf_recursive_check(NCAR(node), env, head);
	if (ret < 0 || ret == RECURSION_INFINITE) return ret;
	r &= ret;
      } while (IS_NOT_NULL(node = NCDR(node)));
    }
    break;

  case NT_QTFR:
    r = subexp_inf_recursive_check(NQTFR(node)->target, env, head);
    if (r == RECURSION_EXIST) {
      if (NQTFR(node)->lower == 0) r = 0;
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
	r = subexp_inf_recursive_check(an->target, env, head);
	break;
      }
    }
    break;

  case NT_CALL:
    r = subexp_inf_recursive_check(NCALL(node)->target, env, head);
    break;

  case NT_ENCLOSE:
    if (IS_ENCLOSE_MARK2(NENCLOSE(node)))
      return 0;
    else if (IS_ENCLOSE_MARK1(NENCLOSE(node)))
      return (head == 0 ? RECURSION_EXIST : RECURSION_INFINITE);
    else {
      SET_ENCLOSE_STATUS(node, NST_MARK2);
      r = subexp_inf_recursive_check(NENCLOSE(node)->target, env, head);
      CLEAR_ENCLOSE_STATUS(node, NST_MARK2);
    }
    break;

  default:
    break;
  }

  return r;
}