static void prepare_uretprobe(struct uprobe *uprobe, struct pt_regs *regs)
{
	struct return_instance *ri;
	struct uprobe_task *utask;
	unsigned long orig_ret_vaddr, trampoline_vaddr;
	bool chained;

	if (!get_xol_area())
		return;

	utask = get_utask();
	if (!utask)
		return;

	if (utask->depth >= MAX_URETPROBE_DEPTH) {
		printk_ratelimited(KERN_INFO "uprobe: omit uretprobe due to"
				" nestedness limit pid/tgid=%d/%d\n",
				current->pid, current->tgid);
		return;
	}

	ri = kmalloc(sizeof(struct return_instance), GFP_KERNEL);
	if (!ri)
		return;

	trampoline_vaddr = get_trampoline_vaddr();
	orig_ret_vaddr = arch_uretprobe_hijack_return_addr(trampoline_vaddr, regs);
	if (orig_ret_vaddr == -1)
		goto fail;

	/* drop the entries invalidated by longjmp() */
	chained = (orig_ret_vaddr == trampoline_vaddr);
	cleanup_return_instances(utask, chained, regs);

	/*
	 * We don't want to keep trampoline address in stack, rather keep the
	 * original return address of first caller thru all the consequent
	 * instances. This also makes breakpoint unwrapping easier.
	 */
	if (chained) {
		if (!utask->return_instances) {
			/*
			 * This situation is not possible. Likely we have an
			 * attack from user-space.
			 */
			uprobe_warn(current, "handle tail call");
			goto fail;
		}
		orig_ret_vaddr = utask->return_instances->orig_ret_vaddr;
	}

	ri->uprobe = get_uprobe(uprobe);
	ri->func = instruction_pointer(regs);
	ri->stack = user_stack_pointer(regs);
	ri->orig_ret_vaddr = orig_ret_vaddr;
	ri->chained = chained;

	utask->depth++;
	ri->next = utask->return_instances;
	utask->return_instances = ri;

	return;
 fail:
	kfree(ri);
}