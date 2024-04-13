static void handle_swbp(struct pt_regs *regs)
{
	struct uprobe *uprobe;
	unsigned long bp_vaddr;
	int uninitialized_var(is_swbp);

	bp_vaddr = uprobe_get_swbp_addr(regs);
	if (bp_vaddr == get_trampoline_vaddr())
		return handle_trampoline(regs);

	uprobe = find_active_uprobe(bp_vaddr, &is_swbp);
	if (!uprobe) {
		if (is_swbp > 0) {
			/* No matching uprobe; signal SIGTRAP. */
			send_sig(SIGTRAP, current, 0);
		} else {
			/*
			 * Either we raced with uprobe_unregister() or we can't
			 * access this memory. The latter is only possible if
			 * another thread plays with our ->mm. In both cases
			 * we can simply restart. If this vma was unmapped we
			 * can pretend this insn was not executed yet and get
			 * the (correct) SIGSEGV after restart.
			 */
			instruction_pointer_set(regs, bp_vaddr);
		}
		return;
	}

	/* change it in advance for ->handler() and restart */
	instruction_pointer_set(regs, bp_vaddr);

	/*
	 * TODO: move copy_insn/etc into _register and remove this hack.
	 * After we hit the bp, _unregister + _register can install the
	 * new and not-yet-analyzed uprobe at the same address, restart.
	 */
	smp_rmb(); /* pairs with wmb() in install_breakpoint() */
	if (unlikely(!test_bit(UPROBE_COPY_INSN, &uprobe->flags)))
		goto out;

	/* Tracing handlers use ->utask to communicate with fetch methods */
	if (!get_utask())
		goto out;

	if (arch_uprobe_ignore(&uprobe->arch, regs))
		goto out;

	handler_chain(uprobe, regs);

	if (arch_uprobe_skip_sstep(&uprobe->arch, regs))
		goto out;

	if (!pre_ssout(uprobe, regs, bp_vaddr))
		return;

	/* arch_uprobe_skip_sstep() succeeded, or restart if can't singlestep */
out:
	put_uprobe(uprobe);
}