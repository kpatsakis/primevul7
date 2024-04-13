compile_range_repeat_node(QtfrNode* qn, int target_len, int empty_info,
                          regex_t* reg)
{
  int r;
  int num_repeat = reg->num_repeat;

  r = add_opcode(reg, qn->greedy ? OP_REPEAT : OP_REPEAT_NG);
  if (r) return r;
  r = add_mem_num(reg, num_repeat); /* OP_REPEAT ID */
  reg->num_repeat++;
  if (r) return r;
  r = add_rel_addr(reg, target_len + SIZE_OP_REPEAT_INC);
  if (r) return r;

  r = entry_repeat_range(reg, num_repeat, qn->lower, qn->upper);
  if (r) return r;

  r = compile_tree_empty_check(qn->target, reg, empty_info);
  if (r) return r;

  if (
#ifdef USE_SUBEXP_CALL
      reg->num_call > 0 ||
#endif
      IS_QUANTIFIER_IN_REPEAT(qn)) {
    r = add_opcode(reg, qn->greedy ? OP_REPEAT_INC_SG : OP_REPEAT_INC_NG_SG);
  }
  else {
    r = add_opcode(reg, qn->greedy ? OP_REPEAT_INC : OP_REPEAT_INC_NG);
  }
  if (r) return r;
  r = add_mem_num(reg, num_repeat); /* OP_REPEAT ID */
  return r;
}