static struct binder_work *binder_dequeue_work_head(
					struct binder_proc *proc,
					struct list_head *list)
{
	struct binder_work *w;

	binder_inner_proc_lock(proc);
	w = binder_dequeue_work_head_ilocked(list);
	binder_inner_proc_unlock(proc);
	return w;
}