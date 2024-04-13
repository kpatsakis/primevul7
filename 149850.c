compile_call(CallNode* node, regex_t* reg)
{
  int r;

  r = add_opcode(reg, OP_CALL);
  if (r) return r;
  r = unset_addr_list_add(node->unset_addr_list, BBUF_GET_OFFSET_POS(reg),
                          node->target);
  if (r) return r;
  r = add_abs_addr(reg, 0 /*dummy addr.*/);
  return r;
}