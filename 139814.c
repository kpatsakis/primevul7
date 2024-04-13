static bool valid_vma(struct vm_area_struct *vma, bool is_register)
{
	vm_flags_t flags = VM_HUGETLB | VM_MAYEXEC | VM_MAYSHARE;

	if (is_register)
		flags |= VM_WRITE;

	return vma->vm_file && (vma->vm_flags & flags) == VM_MAYEXEC;
}