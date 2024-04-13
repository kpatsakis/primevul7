	__acquires(RCU)
{
	struct ct_iter_state *st = seq->private;

	st->time_now = ktime_get_real_ns();
	rcu_read_lock();

	nf_conntrack_get_ht(&st->hash, &st->htable_size);
	return ct_get_idx(seq, *pos);
}