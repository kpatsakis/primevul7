static struct uprobe *get_uprobe(struct uprobe *uprobe)
{
	atomic_inc(&uprobe->ref);
	return uprobe;
}