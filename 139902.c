static int prepare_uprobe(struct uprobe *uprobe, struct file *file,
				struct mm_struct *mm, unsigned long vaddr)
{
	int ret = 0;

	if (test_bit(UPROBE_COPY_INSN, &uprobe->flags))
		return ret;

	/* TODO: move this into _register, until then we abuse this sem. */
	down_write(&uprobe->consumer_rwsem);
	if (test_bit(UPROBE_COPY_INSN, &uprobe->flags))
		goto out;

	ret = copy_insn(uprobe, file);
	if (ret)
		goto out;

	ret = -ENOTSUPP;
	if (is_trap_insn((uprobe_opcode_t *)&uprobe->arch.insn))
		goto out;

	ret = arch_uprobe_analyze_insn(&uprobe->arch, mm, vaddr);
	if (ret)
		goto out;

	/* uprobe_write_opcode() assumes we don't cross page boundary */
	BUG_ON((uprobe->offset & ~PAGE_MASK) +
			UPROBE_SWBP_INSN_SIZE > PAGE_SIZE);

	smp_wmb(); /* pairs with rmb() in find_active_uprobe() */
	set_bit(UPROBE_COPY_INSN, &uprobe->flags);

 out:
	up_write(&uprobe->consumer_rwsem);

	return ret;
}