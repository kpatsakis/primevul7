remove_breakpoint(struct uprobe *uprobe, struct mm_struct *mm, unsigned long vaddr)
{
	set_bit(MMF_RECALC_UPROBES, &mm->flags);
	return set_orig_insn(&uprobe->arch, mm, vaddr);
}