static void binder_free_proc(struct binder_proc *proc)
{
	BUG_ON(!list_empty(&proc->todo));
	BUG_ON(!list_empty(&proc->delivered_death));
	binder_alloc_deferred_release(&proc->alloc);
	put_task_struct(proc->tsk);
	binder_stats_deleted(BINDER_STAT_PROC);
	kfree(proc);
}