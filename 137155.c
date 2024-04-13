BPF_CALL_5(bpf_getsockopt, struct bpf_sock_ops_kern *, bpf_sock,
	   int, level, int, optname, char *, optval, int, optlen)
{
	struct sock *sk = bpf_sock->sk;

	if (!sk_fullsock(sk))
		goto err_clear;

#ifdef CONFIG_INET
	if (level == SOL_TCP && sk->sk_prot->getsockopt == tcp_getsockopt) {
		if (optname == TCP_CONGESTION) {
			struct inet_connection_sock *icsk = inet_csk(sk);

			if (!icsk->icsk_ca_ops || optlen <= 1)
				goto err_clear;
			strncpy(optval, icsk->icsk_ca_ops->name, optlen);
			optval[optlen - 1] = 0;
		} else {
			goto err_clear;
		}
	} else if (level == SOL_IP) {
		struct inet_sock *inet = inet_sk(sk);

		if (optlen != sizeof(int) || sk->sk_family != AF_INET)
			goto err_clear;

		/* Only some options are supported */
		switch (optname) {
		case IP_TOS:
			*((int *)optval) = (int)inet->tos;
			break;
		default:
			goto err_clear;
		}
#if IS_ENABLED(CONFIG_IPV6)
	} else if (level == SOL_IPV6) {
		struct ipv6_pinfo *np = inet6_sk(sk);

		if (optlen != sizeof(int) || sk->sk_family != AF_INET6)
			goto err_clear;

		/* Only some options are supported */
		switch (optname) {
		case IPV6_TCLASS:
			*((int *)optval) = (int)np->tclass;
			break;
		default:
			goto err_clear;
		}
#endif
	} else {
		goto err_clear;
	}
	return 0;
#endif
err_clear:
	memset(optval, 0, optlen);
	return -EINVAL;
}