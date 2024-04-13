onig_print_statistics(FILE* f)
{
  int i;
  fprintf(f, "   count      prev        time\n");
  for (i = 0; OnigOpInfo[i].opcode >= 0; i++) {
    fprintf(f, "%8d: %8d: %10lu: %s\n",
	    OpCounter[i], OpPrevCounter[i], OpTime[i], OnigOpInfo[i].name);
  }
  fprintf(f, "\nmax stack depth: %d\n", MaxStackDepth);
}