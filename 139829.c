static void handle_trampoline(struct pt_regs *regs)
{
	struct uprobe_task *utask;
	struct return_instance *ri, *next;
	bool valid;

	utask = current->utask;
	if (!utask)
		goto sigill;

	ri = utask->return_instances;
	if (!ri)
		goto sigill;

	do {
		/*
		 * We should throw out the frames invalidated by longjmp().
		 * If this chain is valid, then the next one should be alive
		 * or NULL; the latter case means that nobody but ri->func
		 * could hit this trampoline on return. TODO: sigaltstack().
		 */
		next = find_next_ret_chain(ri);
		valid = !next || arch_uretprobe_is_alive(next, RP_CHECK_RET, regs);

		instruction_pointer_set(regs, ri->orig_ret_vaddr);
		do {
			if (valid)
				handle_uretprobe_chain(ri, regs);
			ri = free_ret_instance(ri);
			utask->depth--;
		} while (ri != next);
	} while (!valid);

	utask->return_instances = ri;
	return;

 sigill:
	uprobe_warn(current, "handle uretprobe, sending SIGILL.");
	force_sig_info(SIGILL, SEND_SIG_FORCED, current);

}