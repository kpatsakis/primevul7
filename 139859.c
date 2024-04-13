static void delete_uprobe(struct uprobe *uprobe)
{
	if (WARN_ON(!uprobe_is_active(uprobe)))
		return;

	spin_lock(&uprobes_treelock);
	rb_erase(&uprobe->rb_node, &uprobes_tree);
	spin_unlock(&uprobes_treelock);
	RB_CLEAR_NODE(&uprobe->rb_node); /* for uprobe_is_active() */
	iput(uprobe->inode);
	put_uprobe(uprobe);
}