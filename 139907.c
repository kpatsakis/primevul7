static void dup_xol_work(struct callback_head *work)
{
	if (current->flags & PF_EXITING)
		return;

	if (!__create_xol_area(current->utask->dup_xol_addr) &&
			!fatal_signal_pending(current))
		uprobe_warn(current, "dup xol area");
}