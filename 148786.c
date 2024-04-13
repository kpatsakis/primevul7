static void binder_inc_node_tmpref(struct binder_node *node)
{
	binder_node_lock(node);
	if (node->proc)
		binder_inner_proc_lock(node->proc);
	else
		spin_lock(&binder_dead_nodes_lock);
	binder_inc_node_tmpref_ilocked(node);
	if (node->proc)
		binder_inner_proc_unlock(node->proc);
	else
		spin_unlock(&binder_dead_nodes_lock);
	binder_node_unlock(node);
}