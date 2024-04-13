void uprobe_unregister(struct inode *inode, loff_t offset, struct uprobe_consumer *uc)
{
	struct uprobe *uprobe;

	uprobe = find_uprobe(inode, offset);
	if (WARN_ON(!uprobe))
		return;

	down_write(&uprobe->register_rwsem);
	__uprobe_unregister(uprobe, uc);
	up_write(&uprobe->register_rwsem);
	put_uprobe(uprobe);
}