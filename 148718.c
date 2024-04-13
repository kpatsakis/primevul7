static int binder_stats_show(struct seq_file *m, void *unused)
{
	struct binder_proc *proc;

	seq_puts(m, "binder stats:\n");

	print_binder_stats(m, "", &binder_stats);

	mutex_lock(&binder_procs_lock);
	hlist_for_each_entry(proc, &binder_procs, proc_node)
		print_binder_proc_stats(m, proc);
	mutex_unlock(&binder_procs_lock);

	return 0;
}