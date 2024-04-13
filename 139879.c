void uprobe_copy_process(struct task_struct *t, unsigned long flags)
{
	struct uprobe_task *utask = current->utask;
	struct mm_struct *mm = current->mm;
	struct xol_area *area;

	t->utask = NULL;

	if (!utask || !utask->return_instances)
		return;

	if (mm == t->mm && !(flags & CLONE_VFORK))
		return;

	if (dup_utask(t, utask))
		return uprobe_warn(t, "dup ret instances");

	/* The task can fork() after dup_xol_work() fails */
	area = mm->uprobes_state.xol_area;
	if (!area)
		return uprobe_warn(t, "dup xol area");

	if (mm == t->mm)
		return;

	t->utask->dup_xol_addr = area->vaddr;
	init_task_work(&t->utask->dup_xol_work, dup_xol_work);
	task_work_add(t, &t->utask->dup_xol_work, true);
}