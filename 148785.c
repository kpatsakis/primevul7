static struct binder_thread *binder_get_txn_from_and_acq_inner(
		struct binder_transaction *t)
{
	struct binder_thread *from;

	from = binder_get_txn_from(t);
	if (!from)
		return NULL;
	binder_inner_proc_lock(from->proc);
	if (t->from) {
		BUG_ON(from != t->from);
		return from;
	}
	binder_inner_proc_unlock(from->proc);
	binder_thread_dec_tmpref(from);
	return NULL;
}