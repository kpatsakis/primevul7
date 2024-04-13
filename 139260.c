static void unix_dgram_peer_wake_disconnect(struct sock *sk,
					    struct sock *other)
{
	struct unix_sock *u, *u_other;

	u = unix_sk(sk);
	u_other = unix_sk(other);
	spin_lock(&u_other->peer_wait.lock);

	if (u->peer_wake.private == other) {
		__remove_wait_queue(&u_other->peer_wait, &u->peer_wake);
		u->peer_wake.private = NULL;
	}

	spin_unlock(&u_other->peer_wait.lock);
}