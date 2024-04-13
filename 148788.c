static int binder_proc_show(struct seq_file *m, void *unused)
{
	struct binder_proc *itr;
	int pid = (unsigned long)m->private;

	mutex_lock(&binder_procs_lock);
	hlist_for_each_entry(itr, &binder_procs, proc_node) {
		if (itr->pid == pid) {
			seq_puts(m, "binder proc state:\n");
			print_binder_proc(m, itr, 1);
		}
	}
	mutex_unlock(&binder_procs_lock);

	return 0;
}