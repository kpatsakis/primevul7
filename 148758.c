static struct binder_thread *binder_get_txn_from(
		struct binder_transaction *t)
{
	struct binder_thread *from;

	spin_lock(&t->lock);
	from = t->from;
	if (from)
		atomic_inc(&from->tmp_ref);
	spin_unlock(&t->lock);
	return from;
}