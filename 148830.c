static int binder_transactions_show(struct seq_file *m, void *unused)
{
	struct binder_proc *proc;

	seq_puts(m, "binder transactions:\n");
	mutex_lock(&binder_procs_lock);
	hlist_for_each_entry(proc, &binder_procs, proc_node)
		print_binder_proc(m, proc, 0);
	mutex_unlock(&binder_procs_lock);

	return 0;
}