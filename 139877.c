static struct uprobe *find_uprobe(struct inode *inode, loff_t offset)
{
	struct uprobe *uprobe;

	spin_lock(&uprobes_treelock);
	uprobe = __find_uprobe(inode, offset);
	spin_unlock(&uprobes_treelock);

	return uprobe;
}