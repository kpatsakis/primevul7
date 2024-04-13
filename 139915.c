void uprobe_clear_state(struct mm_struct *mm)
{
	struct xol_area *area = mm->uprobes_state.xol_area;

	if (!area)
		return;

	put_page(area->pages[0]);
	kfree(area->bitmap);
	kfree(area);
}