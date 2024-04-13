int uprobe_mmap(struct vm_area_struct *vma)
{
	struct list_head tmp_list;
	struct uprobe *uprobe, *u;
	struct inode *inode;

	if (no_uprobe_events() || !valid_vma(vma, true))
		return 0;

	inode = file_inode(vma->vm_file);
	if (!inode)
		return 0;

	mutex_lock(uprobes_mmap_hash(inode));
	build_probe_list(inode, vma, vma->vm_start, vma->vm_end, &tmp_list);
	/*
	 * We can race with uprobe_unregister(), this uprobe can be already
	 * removed. But in this case filter_chain() must return false, all
	 * consumers have gone away.
	 */
	list_for_each_entry_safe(uprobe, u, &tmp_list, pending_list) {
		if (!fatal_signal_pending(current) &&
		    filter_chain(uprobe, UPROBE_FILTER_MMAP, vma->vm_mm)) {
			unsigned long vaddr = offset_to_vaddr(vma, uprobe->offset);
			install_breakpoint(uprobe, vma->vm_mm, vma, vaddr);
		}
		put_uprobe(uprobe);
	}
	mutex_unlock(uprobes_mmap_hash(inode));

	return 0;
}