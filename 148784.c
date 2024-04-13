static void binder_free_thread(struct binder_thread *thread)
{
	BUG_ON(!list_empty(&thread->todo));
	binder_stats_deleted(BINDER_STAT_THREAD);
	binder_proc_dec_tmpref(thread->proc);
	kfree(thread);
}