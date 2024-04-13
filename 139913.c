static int __uprobe_register(struct uprobe *uprobe, struct uprobe_consumer *uc)
{
	consumer_add(uprobe, uc);
	return register_for_each_vma(uprobe, uc);
}