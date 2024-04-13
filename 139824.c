register_for_each_vma(struct uprobe *uprobe, struct uprobe_consumer *new)
{
	bool is_register = !!new;
	struct map_info *info;
	int err = 0;

	percpu_down_write(&dup_mmap_sem);
	info = build_map_info(uprobe->inode->i_mapping,
					uprobe->offset, is_register);
	if (IS_ERR(info)) {
		err = PTR_ERR(info);
		goto out;
	}

	while (info) {
		struct mm_struct *mm = info->mm;
		struct vm_area_struct *vma;

		if (err && is_register)
			goto free;

		down_write(&mm->mmap_sem);
		vma = find_vma(mm, info->vaddr);
		if (!vma || !valid_vma(vma, is_register) ||
		    file_inode(vma->vm_file) != uprobe->inode)
			goto unlock;

		if (vma->vm_start > info->vaddr ||
		    vaddr_to_offset(vma, info->vaddr) != uprobe->offset)
			goto unlock;

		if (is_register) {
			/* consult only the "caller", new consumer. */
			if (consumer_filter(new,
					UPROBE_FILTER_REGISTER, mm))
				err = install_breakpoint(uprobe, mm, vma, info->vaddr);
		} else if (test_bit(MMF_HAS_UPROBES, &mm->flags)) {
			if (!filter_chain(uprobe,
					UPROBE_FILTER_UNREGISTER, mm))
				err |= remove_breakpoint(uprobe, mm, info->vaddr);
		}

 unlock:
		up_write(&mm->mmap_sem);
 free:
		mmput(mm);
		info = free_map_info(info);
	}
 out:
	percpu_up_write(&dup_mmap_sem);
	return err;
}