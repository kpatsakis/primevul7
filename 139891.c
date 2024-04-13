static loff_t vaddr_to_offset(struct vm_area_struct *vma, unsigned long vaddr)
{
	return ((loff_t)vma->vm_pgoff << PAGE_SHIFT) + (vaddr - vma->vm_start);
}