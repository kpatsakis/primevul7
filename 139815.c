static void mmf_recalc_uprobes(struct mm_struct *mm)
{
	struct vm_area_struct *vma;

	for (vma = mm->mmap; vma; vma = vma->vm_next) {
		if (!valid_vma(vma, false))
			continue;
		/*
		 * This is not strictly accurate, we can race with
		 * uprobe_unregister() and see the already removed
		 * uprobe if delete_uprobe() was not yet called.
		 * Or this uprobe can be filtered out.
		 */
		if (vma_has_uprobes(vma, vma->vm_start, vma->vm_end))
			return;
	}

	clear_bit(MMF_HAS_UPROBES, &mm->flags);
}