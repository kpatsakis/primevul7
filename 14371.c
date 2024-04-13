static int esp_output_tcp_finish(struct xfrm_state *x, struct sk_buff *skb)
{
	struct sock *sk;
	int err;

	rcu_read_lock();

	sk = esp6_find_tcp_sk(x);
	err = PTR_ERR_OR_ZERO(sk);
	if (err)
		goto out;

	bh_lock_sock(sk);
	if (sock_owned_by_user(sk))
		err = espintcp_queue_out(sk, skb);
	else
		err = espintcp_push_skb(sk, skb);
	bh_unlock_sock(sk);

out:
	rcu_read_unlock();
	return err;
}