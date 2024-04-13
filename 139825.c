static int xol_add_vma(struct mm_struct *mm, struct xol_area *area)
{
	struct vm_area_struct *vma;
	int ret;

	if (down_write_killable(&mm->mmap_sem))
		return -EINTR;

	if (mm->uprobes_state.xol_area) {
		ret = -EALREADY;
		goto fail;
	}

	if (!area->vaddr) {
		/* Try to map as high as possible, this is only a hint. */
		area->vaddr = get_unmapped_area(NULL, TASK_SIZE - PAGE_SIZE,
						PAGE_SIZE, 0, 0);
		if (area->vaddr & ~PAGE_MASK) {
			ret = area->vaddr;
			goto fail;
		}
	}

	vma = _install_special_mapping(mm, area->vaddr, PAGE_SIZE,
				VM_EXEC|VM_MAYEXEC|VM_DONTCOPY|VM_IO,
				&area->xol_mapping);
	if (IS_ERR(vma)) {
		ret = PTR_ERR(vma);
		goto fail;
	}

	ret = 0;
	smp_wmb();	/* pairs with get_xol_area() */
	mm->uprobes_state.xol_area = area;
 fail:
	up_write(&mm->mmap_sem);

	return ret;
}