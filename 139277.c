static void unix_release_sock(struct sock *sk, int embrion)
{
	struct unix_sock *u = unix_sk(sk);
	struct path path;
	struct sock *skpair;
	struct sk_buff *skb;
	int state;

	unix_remove_socket(sk);

	/* Clear state */
	unix_state_lock(sk);
	sock_orphan(sk);
	sk->sk_shutdown = SHUTDOWN_MASK;
	path	     = u->path;
	u->path.dentry = NULL;
	u->path.mnt = NULL;
	state = sk->sk_state;
	sk->sk_state = TCP_CLOSE;
	unix_state_unlock(sk);

	wake_up_interruptible_all(&u->peer_wait);

	skpair = unix_peer(sk);

	if (skpair != NULL) {
		if (sk->sk_type == SOCK_STREAM || sk->sk_type == SOCK_SEQPACKET) {
			unix_state_lock(skpair);
			/* No more writes */
			skpair->sk_shutdown = SHUTDOWN_MASK;
			if (!skb_queue_empty(&sk->sk_receive_queue) || embrion)
				skpair->sk_err = ECONNRESET;
			unix_state_unlock(skpair);
			skpair->sk_state_change(skpair);
			sk_wake_async(skpair, SOCK_WAKE_WAITD, POLL_HUP);
		}

		unix_dgram_peer_wake_disconnect(sk, skpair);
		sock_put(skpair); /* It may now die */
		unix_peer(sk) = NULL;
	}

	/* Try to flush out this socket. Throw out buffers at least */

	while ((skb = skb_dequeue(&sk->sk_receive_queue)) != NULL) {
		if (state == TCP_LISTEN)
			unix_release_sock(skb->sk, 1);
		/* passed fds are erased in the kfree_skb hook	      */
		UNIXCB(skb).consumed = skb->len;
		kfree_skb(skb);
	}

	if (path.dentry)
		path_put(&path);

	sock_put(sk);

	/* ---- Socket is dead now and most probably destroyed ---- */

	/*
	 * Fixme: BSD difference: In BSD all sockets connected to us get
	 *	  ECONNRESET and we die on the spot. In Linux we behave
	 *	  like files and pipes do and wait for the last
	 *	  dereference.
	 *
	 * Can't we simply set sock->err?
	 *
	 *	  What the above comment does talk about? --ANK(980817)
	 */

	if (unix_tot_inflight)
		unix_gc();		/* Garbage collect fds */
}