static int binder_node_release(struct binder_node *node, int refs)
{
	struct binder_ref *ref;
	int death = 0;
	struct binder_proc *proc = node->proc;

	binder_release_work(proc, &node->async_todo);

	binder_node_lock(node);
	binder_inner_proc_lock(proc);
	binder_dequeue_work_ilocked(&node->work);
	/*
	 * The caller must have taken a temporary ref on the node,
	 */
	BUG_ON(!node->tmp_refs);
	if (hlist_empty(&node->refs) && node->tmp_refs == 1) {
		binder_inner_proc_unlock(proc);
		binder_node_unlock(node);
		binder_free_node(node);

		return refs;
	}

	node->proc = NULL;
	node->local_strong_refs = 0;
	node->local_weak_refs = 0;
	binder_inner_proc_unlock(proc);

	spin_lock(&binder_dead_nodes_lock);
	hlist_add_head(&node->dead_node, &binder_dead_nodes);
	spin_unlock(&binder_dead_nodes_lock);

	hlist_for_each_entry(ref, &node->refs, node_entry) {
		refs++;
		/*
		 * Need the node lock to synchronize
		 * with new notification requests and the
		 * inner lock to synchronize with queued
		 * death notifications.
		 */
		binder_inner_proc_lock(ref->proc);
		if (!ref->death) {
			binder_inner_proc_unlock(ref->proc);
			continue;
		}

		death++;

		BUG_ON(!list_empty(&ref->death->work.entry));
		ref->death->work.type = BINDER_WORK_DEAD_BINDER;
		binder_enqueue_work_ilocked(&ref->death->work,
					    &ref->proc->todo);
		binder_wakeup_proc_ilocked(ref->proc);
		binder_inner_proc_unlock(ref->proc);
	}

	binder_debug(BINDER_DEBUG_DEAD_BINDER,
		     "node %d now dead, refs %d, death %d\n",
		     node->debug_id, refs, death);
	binder_node_unlock(node);
	binder_put_node(node);

	return refs;
}