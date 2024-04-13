static int unix_dgram_peer_wake_relay(wait_queue_t *q, unsigned mode, int flags,
				      void *key)
{
	struct unix_sock *u;
	wait_queue_head_t *u_sleep;

	u = container_of(q, struct unix_sock, peer_wake);

	__remove_wait_queue(&unix_sk(u->peer_wake.private)->peer_wait,
			    q);
	u->peer_wake.private = NULL;

	/* relaying can only happen while the wq still exists */
	u_sleep = sk_sleep(&u->sk);
	if (u_sleep)
		wake_up_interruptible_poll(u_sleep, key);

	return 0;
}