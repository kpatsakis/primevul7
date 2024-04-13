static bool binder_has_work(struct binder_thread *thread, bool do_proc_work)
{
	bool has_work;

	binder_inner_proc_lock(thread->proc);
	has_work = binder_has_work_ilocked(thread, do_proc_work);
	binder_inner_proc_unlock(thread->proc);

	return has_work;
}