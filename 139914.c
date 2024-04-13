void uprobe_free_utask(struct task_struct *t)
{
	struct uprobe_task *utask = t->utask;
	struct return_instance *ri;

	if (!utask)
		return;

	if (utask->active_uprobe)
		put_uprobe(utask->active_uprobe);

	ri = utask->return_instances;
	while (ri)
		ri = free_ret_instance(ri);

	xol_free_insn_slot(t);
	kfree(utask);
	t->utask = NULL;
}