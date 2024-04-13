build_map_info(struct address_space *mapping, loff_t offset, bool is_register)
{
	unsigned long pgoff = offset >> PAGE_SHIFT;
	struct vm_area_struct *vma;
	struct map_info *curr = NULL;
	struct map_info *prev = NULL;
	struct map_info *info;
	int more = 0;

 again:
	i_mmap_lock_read(mapping);
	vma_interval_tree_foreach(vma, &mapping->i_mmap, pgoff, pgoff) {
		if (!valid_vma(vma, is_register))
			continue;

		if (!prev && !more) {
			/*
			 * Needs GFP_NOWAIT to avoid i_mmap_rwsem recursion through
			 * reclaim. This is optimistic, no harm done if it fails.
			 */
			prev = kmalloc(sizeof(struct map_info),
					GFP_NOWAIT | __GFP_NOMEMALLOC | __GFP_NOWARN);
			if (prev)
				prev->next = NULL;
		}
		if (!prev) {
			more++;
			continue;
		}

		if (!mmget_not_zero(vma->vm_mm))
			continue;

		info = prev;
		prev = prev->next;
		info->next = curr;
		curr = info;

		info->mm = vma->vm_mm;
		info->vaddr = offset_to_vaddr(vma, offset);
	}
	i_mmap_unlock_read(mapping);

	if (!more)
		goto out;

	prev = curr;
	while (curr) {
		mmput(curr->mm);
		curr = curr->next;
	}

	do {
		info = kmalloc(sizeof(struct map_info), GFP_KERNEL);
		if (!info) {
			curr = ERR_PTR(-ENOMEM);
			goto out;
		}
		info->next = prev;
		prev = info;
	} while (--more);

	goto again;
 out:
	while (prev)
		prev = free_map_info(prev);
	return curr;
}