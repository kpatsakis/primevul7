int __weak set_swbp(struct arch_uprobe *auprobe, struct mm_struct *mm, unsigned long vaddr)
{
	return uprobe_write_opcode(mm, vaddr, UPROBE_SWBP_INSN);
}