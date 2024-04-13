static int unapply_uprobe(struct uprobe *uprobe, struct mm_struct *mm)
{
	struct vm_area_struct *vma;
	int err = 0;

	down_read(&mm->mmap_sem);
	for (vma = mm->mmap; vma; vma = vma->vm_next) {
		unsigned long vaddr;
		loff_t offset;

		if (!valid_vma(vma, false) ||
		    file_inode(vma->vm_file) != uprobe->inode)
			continue;

		offset = (loff_t)vma->vm_pgoff << PAGE_SHIFT;
		if (uprobe->offset <  offset ||
		    uprobe->offset >= offset + vma->vm_end - vma->vm_start)
			continue;

		vaddr = offset_to_vaddr(vma, uprobe->offset);
		err |= remove_breakpoint(uprobe, mm, vaddr);
	}
	up_read(&mm->mmap_sem);

	return err;
}