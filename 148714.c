static void print_binder_proc(struct seq_file *m,
			      struct binder_proc *proc, int print_all)
{
	struct binder_work *w;
	struct rb_node *n;
	size_t start_pos = m->count;
	size_t header_pos;
	struct binder_node *last_node = NULL;

	seq_printf(m, "proc %d\n", proc->pid);
	seq_printf(m, "context %s\n", proc->context->name);
	header_pos = m->count;

	binder_inner_proc_lock(proc);
	for (n = rb_first(&proc->threads); n != NULL; n = rb_next(n))
		print_binder_thread_ilocked(m, rb_entry(n, struct binder_thread,
						rb_node), print_all);

	for (n = rb_first(&proc->nodes); n != NULL; n = rb_next(n)) {
		struct binder_node *node = rb_entry(n, struct binder_node,
						    rb_node);
		/*
		 * take a temporary reference on the node so it
		 * survives and isn't removed from the tree
		 * while we print it.
		 */
		binder_inc_node_tmpref_ilocked(node);
		/* Need to drop inner lock to take node lock */
		binder_inner_proc_unlock(proc);
		if (last_node)
			binder_put_node(last_node);
		binder_node_inner_lock(node);
		print_binder_node_nilocked(m, node);
		binder_node_inner_unlock(node);
		last_node = node;
		binder_inner_proc_lock(proc);
	}
	binder_inner_proc_unlock(proc);
	if (last_node)
		binder_put_node(last_node);

	if (print_all) {
		binder_proc_lock(proc);
		for (n = rb_first(&proc->refs_by_desc);
		     n != NULL;
		     n = rb_next(n))
			print_binder_ref_olocked(m, rb_entry(n,
							    struct binder_ref,
							    rb_node_desc));
		binder_proc_unlock(proc);
	}
	binder_alloc_print_allocated(m, &proc->alloc);
	binder_inner_proc_lock(proc);
	list_for_each_entry(w, &proc->todo, entry)
		print_binder_work_ilocked(m, proc, "  ",
					  "  pending transaction", w);
	list_for_each_entry(w, &proc->delivered_death, entry) {
		seq_puts(m, "  has delivered dead binder\n");
		break;
	}
	binder_inner_proc_unlock(proc);
	if (!print_all && m->count == header_pos)
		m->count = start_pos;
}