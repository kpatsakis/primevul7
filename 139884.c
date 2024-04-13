install_breakpoint(struct uprobe *uprobe, struct mm_struct *mm,
			struct vm_area_struct *vma, unsigned long vaddr)
{
	bool first_uprobe;
	int ret;

	ret = prepare_uprobe(uprobe, vma->vm_file, mm, vaddr);
	if (ret)
		return ret;

	/*
	 * set MMF_HAS_UPROBES in advance for uprobe_pre_sstep_notifier(),
	 * the task can hit this breakpoint right after __replace_page().
	 */
	first_uprobe = !test_bit(MMF_HAS_UPROBES, &mm->flags);
	if (first_uprobe)
		set_bit(MMF_HAS_UPROBES, &mm->flags);

	ret = set_swbp(&uprobe->arch, mm, vaddr);
	if (!ret)
		clear_bit(MMF_RECALC_UPROBES, &mm->flags);
	else if (first_uprobe)
		clear_bit(MMF_HAS_UPROBES, &mm->flags);

	return ret;
}