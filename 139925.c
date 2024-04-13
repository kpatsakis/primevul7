static unsigned long get_trampoline_vaddr(void)
{
	struct xol_area *area;
	unsigned long trampoline_vaddr = -1;

	area = current->mm->uprobes_state.xol_area;
	smp_read_barrier_depends();
	if (area)
		trampoline_vaddr = area->vaddr;

	return trampoline_vaddr;
}