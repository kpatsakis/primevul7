static unsigned long xol_get_insn_slot(struct uprobe *uprobe)
{
	struct xol_area *area;
	unsigned long xol_vaddr;

	area = get_xol_area();
	if (!area)
		return 0;

	xol_vaddr = xol_take_insn_slot(area);
	if (unlikely(!xol_vaddr))
		return 0;

	arch_uprobe_copy_ixol(area->pages[0], xol_vaddr,
			      &uprobe->arch.ixol, sizeof(uprobe->arch.ixol));

	return xol_vaddr;
}