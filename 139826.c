static struct uprobe *find_active_uprobe(unsigned long bp_vaddr, int *is_swbp)
{
	struct mm_struct *mm = current->mm;
	struct uprobe *uprobe = NULL;
	struct vm_area_struct *vma;

	down_read(&mm->mmap_sem);
	vma = find_vma(mm, bp_vaddr);
	if (vma && vma->vm_start <= bp_vaddr) {
		if (valid_vma(vma, false)) {
			struct inode *inode = file_inode(vma->vm_file);
			loff_t offset = vaddr_to_offset(vma, bp_vaddr);

			uprobe = find_uprobe(inode, offset);
		}

		if (!uprobe)
			*is_swbp = is_trap_at_addr(mm, bp_vaddr);
	} else {
		*is_swbp = -EFAULT;
	}

	if (!uprobe && test_and_clear_bit(MMF_RECALC_UPROBES, &mm->flags))
		mmf_recalc_uprobes(mm);
	up_read(&mm->mmap_sem);

	return uprobe;
}