BPF_CALL_2(bpf_sock_ops_cb_flags_set, struct bpf_sock_ops_kern *, bpf_sock,
	   int, argval)
{
	struct sock *sk = bpf_sock->sk;
	int val = argval & BPF_SOCK_OPS_ALL_CB_FLAGS;

	if (!IS_ENABLED(CONFIG_INET) || !sk_fullsock(sk))
		return -EINVAL;

	if (val)
		tcp_sk(sk)->bpf_sock_ops_cb_flags = val;

	return argval & (~BPF_SOCK_OPS_ALL_CB_FLAGS);
}