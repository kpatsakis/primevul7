static void handler_chain(struct uprobe *uprobe, struct pt_regs *regs)
{
	struct uprobe_consumer *uc;
	int remove = UPROBE_HANDLER_REMOVE;
	bool need_prep = false; /* prepare return uprobe, when needed */

	down_read(&uprobe->register_rwsem);
	for (uc = uprobe->consumers; uc; uc = uc->next) {
		int rc = 0;

		if (uc->handler) {
			rc = uc->handler(uc, regs);
			WARN(rc & ~UPROBE_HANDLER_MASK,
				"bad rc=0x%x from %pf()\n", rc, uc->handler);
		}

		if (uc->ret_handler)
			need_prep = true;

		remove &= rc;
	}

	if (need_prep && !remove)
		prepare_uretprobe(uprobe, regs); /* put bp at return */

	if (remove && uprobe->consumers) {
		WARN_ON(!uprobe_is_active(uprobe));
		unapply_uprobe(uprobe, current->mm);
	}
	up_read(&uprobe->register_rwsem);
}