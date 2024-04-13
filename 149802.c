op2arg_type(int opcode)
{
  int i;

  for (i = 0; OnigOpInfo[i].opcode >= 0; i++) {
    if (opcode == OnigOpInfo[i].opcode)
      return OnigOpInfo[i].arg_type;
  }
  return ARG_SPECIAL;
}