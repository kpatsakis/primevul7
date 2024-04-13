binder_enqueue_thread_work_ilocked(struct binder_thread *thread,
				   struct binder_work *work)
{
	binder_enqueue_work_ilocked(work, &thread->todo);
	thread->process_todo = true;
}