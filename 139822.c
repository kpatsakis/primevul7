static void __uprobe_unregister(struct uprobe *uprobe, struct uprobe_consumer *uc)
{
	int err;

	if (WARN_ON(!consumer_del(uprobe, uc)))
		return;

	err = register_for_each_vma(uprobe, NULL);
	/* TODO : cant unregister? schedule a worker thread */
	if (!uprobe->consumers && !err)
		delete_uprobe(uprobe);
}