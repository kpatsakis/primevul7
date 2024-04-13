static bool binder_has_work_ilocked(struct binder_thread *thread,
				    bool do_proc_work)
{
	return thread->process_todo ||
		thread->looper_need_return ||
		(do_proc_work &&
		 !binder_worklist_empty_ilocked(&thread->proc->todo));
}