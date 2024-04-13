is_left_anchor(int anc)
{
  if (anc == ANCHOR_END_BUF || anc == ANCHOR_SEMI_END_BUF ||
      anc == ANCHOR_END_LINE || anc == ANCHOR_PREC_READ ||
      anc == ANCHOR_PREC_READ_NOT)
    return 0;

  return 1;
}