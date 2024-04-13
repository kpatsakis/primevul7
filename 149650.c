compile_length_quantifier_node(QtfrNode* qn, regex_t* reg)
{
  int len, mod_tlen, cklen;
  int ckn;
  int infinite = IS_REPEAT_INFINITE(qn->upper);
  int empty_info = qn->target_empty_info;
  int tlen = compile_length_tree(qn->target, reg);

  if (tlen < 0) return tlen;

  ckn = ((reg->num_comb_exp_check > 0) ? qn->comb_exp_check_num : 0);

  cklen = (CKN_ON ? SIZE_STATE_CHECK_NUM: 0);

  /* anychar repeat */
  if (NTYPE(qn->target) == NT_CANY) {
    if (qn->greedy && infinite) {
      if (IS_NOT_NULL(qn->next_head_exact) && !CKN_ON)
	return SIZE_OP_ANYCHAR_STAR_PEEK_NEXT + tlen * qn->lower + cklen;
      else
	return SIZE_OP_ANYCHAR_STAR + tlen * qn->lower + cklen;
    }
  }

  if (empty_info != 0)
    mod_tlen = tlen + (SIZE_OP_NULL_CHECK_START + SIZE_OP_NULL_CHECK_END);
  else
    mod_tlen = tlen;

  if (infinite && qn->lower <= 1) {
    if (qn->greedy) {
      if (qn->lower == 1)
	len = SIZE_OP_JUMP;
      else
	len = 0;

      len += SIZE_OP_PUSH + cklen + mod_tlen + SIZE_OP_JUMP;
    }
    else {
      if (qn->lower == 0)
	len = SIZE_OP_JUMP;
      else
	len = 0;

      len += mod_tlen + SIZE_OP_PUSH + cklen;
    }
  }
  else if (qn->upper == 0) {
    if (qn->is_refered != 0) /* /(?<n>..){0}/ */
      len = SIZE_OP_JUMP + tlen;
    else
      len = 0;
  }
  else if (qn->upper == 1 && qn->greedy) {
    if (qn->lower == 0) {
      if (CKN_ON) {
	len = SIZE_OP_STATE_CHECK_PUSH + tlen;
      }
      else {
	len = SIZE_OP_PUSH + tlen;
      }
    }
    else {
      len = tlen;
    }
  }
  else if (!qn->greedy && qn->upper == 1 && qn->lower == 0) { /* '??' */
    len = SIZE_OP_PUSH + cklen + SIZE_OP_JUMP + tlen;
  }
  else {
    len = SIZE_OP_REPEAT_INC
        + mod_tlen + SIZE_OPCODE + SIZE_RELADDR + SIZE_MEMNUM;
    if (CKN_ON)
      len += SIZE_OP_STATE_CHECK;
  }

  return len;
}