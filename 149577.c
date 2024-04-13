numbered_ref_check(Node* node)
{
  int r = 0;

  switch (NTYPE(node)) {
  case NT_LIST:
  case NT_ALT:
    do {
      r = numbered_ref_check(NCAR(node));
    } while (r == 0 && IS_NOT_NULL(node = NCDR(node)));
    break;
  case NT_QTFR:
    r = numbered_ref_check(NQTFR(node)->target);
    break;
  case NT_ENCLOSE:
    r = numbered_ref_check(NENCLOSE(node)->target);
    break;

  case NT_BREF:
    if (! IS_BACKREF_NAME_REF(NBREF(node)))
      return ONIGERR_NUMBERED_BACKREF_OR_CALL_NOT_ALLOWED;
    break;

  case NT_ANCHOR:
    if (NANCHOR(node)->target)
      r = numbered_ref_check(NANCHOR(node)->target);
    break;

  default:
    break;
  }

  return r;
}