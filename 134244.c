static struct kioctx *lookup_ioctx(unsigned long ctx_id)
{
	struct aio_ring __user *ring  = (void __user *)ctx_id;
	struct mm_struct *mm = current->mm;
	struct kioctx *ctx, *ret = NULL;
	struct kioctx_table *table;
	unsigned id;

	if (get_user(id, &ring->id))
		return NULL;

	rcu_read_lock();
	table = rcu_dereference(mm->ioctx_table);

	if (!table || id >= table->nr)
		goto out;

	id = array_index_nospec(id, table->nr);
	ctx = rcu_dereference(table->table[id]);
	if (ctx && ctx->user_id == ctx_id) {
		if (percpu_ref_tryget_live(&ctx->users))
			ret = ctx;
	}
out:
	rcu_read_unlock();
	return ret;
}