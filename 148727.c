_binder_node_inner_lock(struct binder_node *node, int line)
{
	binder_debug(BINDER_DEBUG_SPINLOCKS,
		     "%s: line=%d\n", __func__, line);
	spin_lock(&node->lock);
	if (node->proc)
		binder_inner_proc_lock(node->proc);
}