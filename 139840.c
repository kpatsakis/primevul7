static void handle_singlestep(struct uprobe_task *utask, struct pt_regs *regs)
{
	struct uprobe *uprobe;
	int err = 0;

	uprobe = utask->active_uprobe;
	if (utask->state == UTASK_SSTEP_ACK)
		err = arch_uprobe_post_xol(&uprobe->arch, regs);
	else if (utask->state == UTASK_SSTEP_TRAPPED)
		arch_uprobe_abort_xol(&uprobe->arch, regs);
	else
		WARN_ON_ONCE(1);

	put_uprobe(uprobe);
	utask->active_uprobe = NULL;
	utask->state = UTASK_RUNNING;
	xol_free_insn_slot(current);

	spin_lock_irq(&current->sighand->siglock);
	recalc_sigpending(); /* see uprobe_deny_signal() */
	spin_unlock_irq(&current->sighand->siglock);

	if (unlikely(err)) {
		uprobe_warn(current, "execute the probed insn, sending SIGILL.");
		force_sig_info(SIGILL, SEND_SIG_FORCED, current);
	}
}