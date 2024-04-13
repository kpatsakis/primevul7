add_compile_string(UChar* s, int mb_len, OnigDistance byte_len,
                   regex_t* reg, int ignore_case)
{
  int op = select_str_opcode(mb_len, byte_len, ignore_case);
  add_opcode(reg, op);

  if (op == OP_EXACTMBN)
    add_length(reg, mb_len);

  if (IS_NEED_STR_LEN_OP_EXACT(op)) {
    if (op == OP_EXACTN_IC)
      add_length(reg, byte_len);
    else
      add_length(reg, byte_len / mb_len);
  }

  add_bytes(reg, s, byte_len);
  return 0;
}