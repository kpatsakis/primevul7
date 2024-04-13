compile_tree_empty_check(Node* node, regex_t* reg, int empty_info)
{
  int r;
  int saved_num_null_check = reg->num_null_check;

  if (empty_info != 0) {
    r = add_opcode(reg, OP_NULL_CHECK_START);
    if (r) return r;
    r = add_mem_num(reg, reg->num_null_check); /* NULL CHECK ID */
    if (r) return r;
    reg->num_null_check++;
  }

  r = compile_tree(node, reg);
  if (r) return r;

  if (empty_info != 0) {
    if (empty_info == NQ_TARGET_IS_EMPTY)
      r = add_opcode(reg, OP_NULL_CHECK_END);
    else if (empty_info == NQ_TARGET_IS_EMPTY_MEM)
      r = add_opcode(reg, OP_NULL_CHECK_END_MEMST);
    else if (empty_info == NQ_TARGET_IS_EMPTY_REC)
      r = add_opcode(reg, OP_NULL_CHECK_END_MEMST_PUSH);

    if (r) return r;
    r = add_mem_num(reg, saved_num_null_check); /* NULL CHECK ID */
  }
  return r;
}