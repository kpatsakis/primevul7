static int unix_writable(const struct sock *sk)
{
	return sk->sk_state != TCP_LISTEN &&
	       (atomic_read(&sk->sk_wmem_alloc) << 2) <= sk->sk_sndbuf;
}