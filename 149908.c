op2name(int opcode)
{
  int i;

  for (i = 0; OnigOpInfo[i].opcode >= 0; i++) {
    if (opcode == OnigOpInfo[i].opcode)
      return OnigOpInfo[i].name;
  }
  return "";
}