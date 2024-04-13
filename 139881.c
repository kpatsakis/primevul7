bool __weak is_trap_insn(uprobe_opcode_t *insn)
{
	return is_swbp_insn(insn);
}