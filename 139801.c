static struct uprobe *insert_uprobe(struct uprobe *uprobe)
{
	struct uprobe *u;

	spin_lock(&uprobes_treelock);
	u = __insert_uprobe(uprobe);
	spin_unlock(&uprobes_treelock);

	return u;
}