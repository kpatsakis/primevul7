unsigned long uprobe_get_trap_addr(struct pt_regs *regs)
{
	struct uprobe_task *utask = current->utask;

	if (unlikely(utask && utask->active_uprobe))
		return utask->vaddr;

	return instruction_pointer(regs);
}