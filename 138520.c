static unsigned int run_filter(struct sk_buff *skb,
			       const struct sock *sk,
			       unsigned int res)
{
	struct sk_filter *filter;

	rcu_read_lock();
	filter = rcu_dereference(sk->sk_filter);
	if (filter != NULL)
		res = bpf_prog_run_clear_cb(filter->prog, skb);
	rcu_read_unlock();

	return res;
}