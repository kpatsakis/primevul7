static struct xol_area *get_xol_area(void)
{
	struct mm_struct *mm = current->mm;
	struct xol_area *area;

	if (!mm->uprobes_state.xol_area)
		__create_xol_area(0);

	area = mm->uprobes_state.xol_area;
	smp_read_barrier_depends();	/* pairs with wmb in xol_add_vma() */
	return area;
}