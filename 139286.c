static int unix_stream_connect(struct socket *sock, struct sockaddr *uaddr,
			       int addr_len, int flags)
{
	struct sockaddr_un *sunaddr = (struct sockaddr_un *)uaddr;
	struct sock *sk = sock->sk;
	struct net *net = sock_net(sk);
	struct unix_sock *u = unix_sk(sk), *newu, *otheru;
	struct sock *newsk = NULL;
	struct sock *other = NULL;
	struct sk_buff *skb = NULL;
	unsigned int hash;
	int st;
	int err;
	long timeo;

	err = unix_mkname(sunaddr, addr_len, &hash);
	if (err < 0)
		goto out;
	addr_len = err;

	if (test_bit(SOCK_PASSCRED, &sock->flags) && !u->addr &&
	    (err = unix_autobind(sock)) != 0)
		goto out;

	timeo = sock_sndtimeo(sk, flags & O_NONBLOCK);

	/* First of all allocate resources.
	   If we will make it after state is locked,
	   we will have to recheck all again in any case.
	 */

	err = -ENOMEM;

	/* create new sock for complete connection */
	newsk = unix_create1(sock_net(sk), NULL, 0);
	if (newsk == NULL)
		goto out;

	/* Allocate skb for sending to listening sock */
	skb = sock_wmalloc(newsk, 1, 0, GFP_KERNEL);
	if (skb == NULL)
		goto out;

restart:
	/*  Find listening sock. */
	other = unix_find_other(net, sunaddr, addr_len, sk->sk_type, hash, &err);
	if (!other)
		goto out;

	/* Latch state of peer */
	unix_state_lock(other);

	/* Apparently VFS overslept socket death. Retry. */
	if (sock_flag(other, SOCK_DEAD)) {
		unix_state_unlock(other);
		sock_put(other);
		goto restart;
	}

	err = -ECONNREFUSED;
	if (other->sk_state != TCP_LISTEN)
		goto out_unlock;
	if (other->sk_shutdown & RCV_SHUTDOWN)
		goto out_unlock;

	if (unix_recvq_full(other)) {
		err = -EAGAIN;
		if (!timeo)
			goto out_unlock;

		timeo = unix_wait_for_peer(other, timeo);

		err = sock_intr_errno(timeo);
		if (signal_pending(current))
			goto out;
		sock_put(other);
		goto restart;
	}

	/* Latch our state.

	   It is tricky place. We need to grab our state lock and cannot
	   drop lock on peer. It is dangerous because deadlock is
	   possible. Connect to self case and simultaneous
	   attempt to connect are eliminated by checking socket
	   state. other is TCP_LISTEN, if sk is TCP_LISTEN we
	   check this before attempt to grab lock.

	   Well, and we have to recheck the state after socket locked.
	 */
	st = sk->sk_state;

	switch (st) {
	case TCP_CLOSE:
		/* This is ok... continue with connect */
		break;
	case TCP_ESTABLISHED:
		/* Socket is already connected */
		err = -EISCONN;
		goto out_unlock;
	default:
		err = -EINVAL;
		goto out_unlock;
	}

	unix_state_lock_nested(sk);

	if (sk->sk_state != st) {
		unix_state_unlock(sk);
		unix_state_unlock(other);
		sock_put(other);
		goto restart;
	}

	err = security_unix_stream_connect(sk, other, newsk);
	if (err) {
		unix_state_unlock(sk);
		goto out_unlock;
	}

	/* The way is open! Fastly set all the necessary fields... */

	sock_hold(sk);
	unix_peer(newsk)	= sk;
	newsk->sk_state		= TCP_ESTABLISHED;
	newsk->sk_type		= sk->sk_type;
	init_peercred(newsk);
	newu = unix_sk(newsk);
	RCU_INIT_POINTER(newsk->sk_wq, &newu->peer_wq);
	otheru = unix_sk(other);

	/* copy address information from listening to new sock*/
	if (otheru->addr) {
		atomic_inc(&otheru->addr->refcnt);
		newu->addr = otheru->addr;
	}
	if (otheru->path.dentry) {
		path_get(&otheru->path);
		newu->path = otheru->path;
	}

	/* Set credentials */
	copy_peercred(sk, other);

	sock->state	= SS_CONNECTED;
	sk->sk_state	= TCP_ESTABLISHED;
	sock_hold(newsk);

	smp_mb__after_atomic();	/* sock_hold() does an atomic_inc() */
	unix_peer(sk)	= newsk;

	unix_state_unlock(sk);

	/* take ten and and send info to listening sock */
	spin_lock(&other->sk_receive_queue.lock);
	__skb_queue_tail(&other->sk_receive_queue, skb);
	spin_unlock(&other->sk_receive_queue.lock);
	unix_state_unlock(other);
	other->sk_data_ready(other);
	sock_put(other);
	return 0;

out_unlock:
	if (other)
		unix_state_unlock(other);

out:
	kfree_skb(skb);
	if (newsk)
		unix_release_sock(newsk, 0);
	if (other)
		sock_put(other);
	return err;
}