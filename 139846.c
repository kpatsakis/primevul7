pre_ssout(struct uprobe *uprobe, struct pt_regs *regs, unsigned long bp_vaddr)
{
	struct uprobe_task *utask;
	unsigned long xol_vaddr;
	int err;

	utask = get_utask();
	if (!utask)
		return -ENOMEM;

	xol_vaddr = xol_get_insn_slot(uprobe);
	if (!xol_vaddr)
		return -ENOMEM;

	utask->xol_vaddr = xol_vaddr;
	utask->vaddr = bp_vaddr;

	err = arch_uprobe_pre_xol(&uprobe->arch, regs);
	if (unlikely(err)) {
		xol_free_insn_slot(current);
		return err;
	}

	utask->active_uprobe = uprobe;
	utask->state = UTASK_SSTEP;
	return 0;
}