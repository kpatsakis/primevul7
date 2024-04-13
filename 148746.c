static void print_binder_thread_ilocked(struct seq_file *m,
					struct binder_thread *thread,
					int print_always)
{
	struct binder_transaction *t;
	struct binder_work *w;
	size_t start_pos = m->count;
	size_t header_pos;

	seq_printf(m, "  thread %d: l %02x need_return %d tr %d\n",
			thread->pid, thread->looper,
			thread->looper_need_return,
			atomic_read(&thread->tmp_ref));
	header_pos = m->count;
	t = thread->transaction_stack;
	while (t) {
		if (t->from == thread) {
			print_binder_transaction_ilocked(m, thread->proc,
					"    outgoing transaction", t);
			t = t->from_parent;
		} else if (t->to_thread == thread) {
			print_binder_transaction_ilocked(m, thread->proc,
						 "    incoming transaction", t);
			t = t->to_parent;
		} else {
			print_binder_transaction_ilocked(m, thread->proc,
					"    bad transaction", t);
			t = NULL;
		}
	}
	list_for_each_entry(w, &thread->todo, entry) {
		print_binder_work_ilocked(m, thread->proc, "    ",
					  "    pending transaction", w);
	}
	if (!print_always && m->count == header_pos)
		m->count = start_pos;
}