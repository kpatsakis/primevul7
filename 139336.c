static struct sock *unix_create1(struct net *net, struct socket *sock, int kern)
{
	struct sock *sk = NULL;
	struct unix_sock *u;

	atomic_long_inc(&unix_nr_socks);
	if (atomic_long_read(&unix_nr_socks) > 2 * get_max_files())
		goto out;

	sk = sk_alloc(net, PF_UNIX, GFP_KERNEL, &unix_proto, kern);
	if (!sk)
		goto out;

	sock_init_data(sock, sk);
	lockdep_set_class(&sk->sk_receive_queue.lock,
				&af_unix_sk_receive_queue_lock_key);

	sk->sk_write_space	= unix_write_space;
	sk->sk_max_ack_backlog	= net->unx.sysctl_max_dgram_qlen;
	sk->sk_destruct		= unix_sock_destructor;
	u	  = unix_sk(sk);
	u->path.dentry = NULL;
	u->path.mnt = NULL;
	spin_lock_init(&u->lock);
	atomic_long_set(&u->inflight, 0);
	INIT_LIST_HEAD(&u->link);
	mutex_init(&u->readlock); /* single task reading lock */
	init_waitqueue_head(&u->peer_wait);
	init_waitqueue_func_entry(&u->peer_wake, unix_dgram_peer_wake_relay);
	unix_insert_socket(unix_sockets_unbound(sk), sk);
out:
	if (sk == NULL)
		atomic_long_dec(&unix_nr_socks);
	else {
		local_bh_disable();
		sock_prot_inuse_add(sock_net(sk), sk->sk_prot, 1);
		local_bh_enable();
	}
	return sk;
}