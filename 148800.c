static bool binder_worklist_empty(struct binder_proc *proc,
				  struct list_head *list)
{
	bool ret;

	binder_inner_proc_lock(proc);
	ret = binder_worklist_empty_ilocked(list);
	binder_inner_proc_unlock(proc);
	return ret;
}