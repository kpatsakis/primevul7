int uprobe_apply(struct inode *inode, loff_t offset,
			struct uprobe_consumer *uc, bool add)
{
	struct uprobe *uprobe;
	struct uprobe_consumer *con;
	int ret = -ENOENT;

	uprobe = find_uprobe(inode, offset);
	if (WARN_ON(!uprobe))
		return ret;

	down_write(&uprobe->register_rwsem);
	for (con = uprobe->consumers; con && con != uc ; con = con->next)
		;
	if (con)
		ret = register_for_each_vma(uprobe, add ? uc : NULL);
	up_write(&uprobe->register_rwsem);
	put_uprobe(uprobe);

	return ret;
}