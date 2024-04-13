bool __weak arch_uprobe_ignore(struct arch_uprobe *aup, struct pt_regs *regs)
{
	return false;
}