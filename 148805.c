static bool binder_proc_transaction(struct binder_transaction *t,
				    struct binder_proc *proc,
				    struct binder_thread *thread)
{
	struct binder_node *node = t->buffer->target_node;
	bool oneway = !!(t->flags & TF_ONE_WAY);
	bool pending_async = false;

	BUG_ON(!node);
	binder_node_lock(node);
	if (oneway) {
		BUG_ON(thread);
		if (node->has_async_transaction) {
			pending_async = true;
		} else {
			node->has_async_transaction = true;
		}
	}

	binder_inner_proc_lock(proc);

	if (proc->is_dead || (thread && thread->is_dead)) {
		binder_inner_proc_unlock(proc);
		binder_node_unlock(node);
		return false;
	}

	if (!thread && !pending_async)
		thread = binder_select_thread_ilocked(proc);

	if (thread)
		binder_enqueue_thread_work_ilocked(thread, &t->work);
	else if (!pending_async)
		binder_enqueue_work_ilocked(&t->work, &proc->todo);
	else
		binder_enqueue_work_ilocked(&t->work, &node->async_todo);

	if (!pending_async)
		binder_wakeup_thread_ilocked(proc, thread, !oneway /* sync */);

	binder_inner_proc_unlock(proc);
	binder_node_unlock(node);

	return true;
}