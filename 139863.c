void uprobe_notify_resume(struct pt_regs *regs)
{
	struct uprobe_task *utask;

	clear_thread_flag(TIF_UPROBE);

	utask = current->utask;
	if (utask && utask->active_uprobe)
		handle_singlestep(utask, regs);
	else
		handle_swbp(regs);
}