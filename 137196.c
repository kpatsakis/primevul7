static int __reuseport_attach_prog(struct bpf_prog *prog, struct sock *sk)
{
	struct bpf_prog *old_prog;
	int err;

	if (bpf_prog_size(prog->len) > sysctl_optmem_max)
		return -ENOMEM;

	if (sk_unhashed(sk) && sk->sk_reuseport) {
		err = reuseport_alloc(sk);
		if (err)
			return err;
	} else if (!rcu_access_pointer(sk->sk_reuseport_cb)) {
		/* The socket wasn't bound with SO_REUSEPORT */
		return -EINVAL;
	}

	old_prog = reuseport_attach_prog(sk, prog);
	if (old_prog)
		bpf_prog_destroy(old_prog);

	return 0;
}