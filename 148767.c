binder_enqueue_thread_work(struct binder_thread *thread,
			   struct binder_work *work)
{
	binder_inner_proc_lock(thread->proc);
	binder_enqueue_thread_work_ilocked(thread, work);
	binder_inner_proc_unlock(thread->proc);
}