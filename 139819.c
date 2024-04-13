int uprobe_register(struct inode *inode, loff_t offset, struct uprobe_consumer *uc)
{
	struct uprobe *uprobe;
	int ret;

	/* Uprobe must have at least one set consumer */
	if (!uc->handler && !uc->ret_handler)
		return -EINVAL;

	/* copy_insn() uses read_mapping_page() or shmem_read_mapping_page() */
	if (!inode->i_mapping->a_ops->readpage && !shmem_mapping(inode->i_mapping))
		return -EIO;
	/* Racy, just to catch the obvious mistakes */
	if (offset > i_size_read(inode))
		return -EINVAL;

 retry:
	uprobe = alloc_uprobe(inode, offset);
	if (!uprobe)
		return -ENOMEM;
	/*
	 * We can race with uprobe_unregister()->delete_uprobe().
	 * Check uprobe_is_active() and retry if it is false.
	 */
	down_write(&uprobe->register_rwsem);
	ret = -EAGAIN;
	if (likely(uprobe_is_active(uprobe))) {
		ret = __uprobe_register(uprobe, uc);
		if (ret)
			__uprobe_unregister(uprobe, uc);
	}
	up_write(&uprobe->register_rwsem);
	put_uprobe(uprobe);

	if (unlikely(ret == -EAGAIN))
		goto retry;
	return ret;
}