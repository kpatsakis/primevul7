static int binder_state_show(struct seq_file *m, void *unused)
{
	struct binder_proc *proc;
	struct binder_node *node;
	struct binder_node *last_node = NULL;

	seq_puts(m, "binder state:\n");

	spin_lock(&binder_dead_nodes_lock);
	if (!hlist_empty(&binder_dead_nodes))
		seq_puts(m, "dead nodes:\n");
	hlist_for_each_entry(node, &binder_dead_nodes, dead_node) {
		/*
		 * take a temporary reference on the node so it
		 * survives and isn't removed from the list
		 * while we print it.
		 */
		node->tmp_refs++;
		spin_unlock(&binder_dead_nodes_lock);
		if (last_node)
			binder_put_node(last_node);
		binder_node_lock(node);
		print_binder_node_nilocked(m, node);
		binder_node_unlock(node);
		last_node = node;
		spin_lock(&binder_dead_nodes_lock);
	}
	spin_unlock(&binder_dead_nodes_lock);
	if (last_node)
		binder_put_node(last_node);

	mutex_lock(&binder_procs_lock);
	hlist_for_each_entry(proc, &binder_procs, proc_node)
		print_binder_proc(m, proc, 1);
	mutex_unlock(&binder_procs_lock);

	return 0;
}