int uprobe_pre_sstep_notifier(struct pt_regs *regs)
{
	if (!current->mm)
		return 0;

	if (!test_bit(MMF_HAS_UPROBES, &current->mm->flags) &&
	    (!current->utask || !current->utask->return_instances))
		return 0;

	set_thread_flag(TIF_UPROBE);
	return 1;
}