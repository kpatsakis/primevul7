long unix_outq_len(struct sock *sk)
{
	return sk_wmem_alloc_get(sk);
}