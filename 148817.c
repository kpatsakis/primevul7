static void print_binder_proc_stats(struct seq_file *m,
				    struct binder_proc *proc)
{
	struct binder_work *w;
	struct binder_thread *thread;
	struct rb_node *n;
	int count, strong, weak, ready_threads;
	size_t free_async_space =
		binder_alloc_get_free_async_space(&proc->alloc);

	seq_printf(m, "proc %d\n", proc->pid);
	seq_printf(m, "context %s\n", proc->context->name);
	count = 0;
	ready_threads = 0;
	binder_inner_proc_lock(proc);
	for (n = rb_first(&proc->threads); n != NULL; n = rb_next(n))
		count++;

	list_for_each_entry(thread, &proc->waiting_threads, waiting_thread_node)
		ready_threads++;

	seq_printf(m, "  threads: %d\n", count);
	seq_printf(m, "  requested threads: %d+%d/%d\n"
			"  ready threads %d\n"
			"  free async space %zd\n", proc->requested_threads,
			proc->requested_threads_started, proc->max_threads,
			ready_threads,
			free_async_space);
	count = 0;
	for (n = rb_first(&proc->nodes); n != NULL; n = rb_next(n))
		count++;
	binder_inner_proc_unlock(proc);
	seq_printf(m, "  nodes: %d\n", count);
	count = 0;
	strong = 0;
	weak = 0;
	binder_proc_lock(proc);
	for (n = rb_first(&proc->refs_by_desc); n != NULL; n = rb_next(n)) {
		struct binder_ref *ref = rb_entry(n, struct binder_ref,
						  rb_node_desc);
		count++;
		strong += ref->data.strong;
		weak += ref->data.weak;
	}
	binder_proc_unlock(proc);
	seq_printf(m, "  refs: %d s %d w %d\n", count, strong, weak);

	count = binder_alloc_get_allocated_count(&proc->alloc);
	seq_printf(m, "  buffers: %d\n", count);

	binder_alloc_print_pages(m, &proc->alloc);

	count = 0;
	binder_inner_proc_lock(proc);
	list_for_each_entry(w, &proc->todo, entry) {
		if (w->type == BINDER_WORK_TRANSACTION)
			count++;
	}
	binder_inner_proc_unlock(proc);
	seq_printf(m, "  pending transactions: %d\n", count);

	print_binder_stats(m, "  ", &proc->stats);
}