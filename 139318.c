static int unix_set_peek_off(struct sock *sk, int val)
{
	struct unix_sock *u = unix_sk(sk);

	if (mutex_lock_interruptible(&u->readlock))
		return -EINTR;

	sk->sk_peek_off = val;
	mutex_unlock(&u->readlock);

	return 0;
}