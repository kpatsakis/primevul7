BPF_CALL_5(bpf_setsockopt, struct bpf_sock_ops_kern *, bpf_sock,
	   int, level, int, optname, char *, optval, int, optlen)
{
	struct sock *sk = bpf_sock->sk;
	int ret = 0;
	int val;

	if (!sk_fullsock(sk))
		return -EINVAL;

	if (level == SOL_SOCKET) {
		if (optlen != sizeof(int))
			return -EINVAL;
		val = *((int *)optval);

		/* Only some socketops are supported */
		switch (optname) {
		case SO_RCVBUF:
			sk->sk_userlocks |= SOCK_RCVBUF_LOCK;
			sk->sk_rcvbuf = max_t(int, val * 2, SOCK_MIN_RCVBUF);
			break;
		case SO_SNDBUF:
			sk->sk_userlocks |= SOCK_SNDBUF_LOCK;
			sk->sk_sndbuf = max_t(int, val * 2, SOCK_MIN_SNDBUF);
			break;
		case SO_MAX_PACING_RATE:
			sk->sk_max_pacing_rate = val;
			sk->sk_pacing_rate = min(sk->sk_pacing_rate,
						 sk->sk_max_pacing_rate);
			break;
		case SO_PRIORITY:
			sk->sk_priority = val;
			break;
		case SO_RCVLOWAT:
			if (val < 0)
				val = INT_MAX;
			sk->sk_rcvlowat = val ? : 1;
			break;
		case SO_MARK:
			sk->sk_mark = val;
			break;
		default:
			ret = -EINVAL;
		}
#ifdef CONFIG_INET
	} else if (level == SOL_IP) {
		if (optlen != sizeof(int) || sk->sk_family != AF_INET)
			return -EINVAL;

		val = *((int *)optval);
		/* Only some options are supported */
		switch (optname) {
		case IP_TOS:
			if (val < -1 || val > 0xff) {
				ret = -EINVAL;
			} else {
				struct inet_sock *inet = inet_sk(sk);

				if (val == -1)
					val = 0;
				inet->tos = val;
			}
			break;
		default:
			ret = -EINVAL;
		}
#if IS_ENABLED(CONFIG_IPV6)
	} else if (level == SOL_IPV6) {
		if (optlen != sizeof(int) || sk->sk_family != AF_INET6)
			return -EINVAL;

		val = *((int *)optval);
		/* Only some options are supported */
		switch (optname) {
		case IPV6_TCLASS:
			if (val < -1 || val > 0xff) {
				ret = -EINVAL;
			} else {
				struct ipv6_pinfo *np = inet6_sk(sk);

				if (val == -1)
					val = 0;
				np->tclass = val;
			}
			break;
		default:
			ret = -EINVAL;
		}
#endif
	} else if (level == SOL_TCP &&
		   sk->sk_prot->setsockopt == tcp_setsockopt) {
		if (optname == TCP_CONGESTION) {
			char name[TCP_CA_NAME_MAX];
			bool reinit = bpf_sock->op > BPF_SOCK_OPS_NEEDS_ECN;

			strncpy(name, optval, min_t(long, optlen,
						    TCP_CA_NAME_MAX-1));
			name[TCP_CA_NAME_MAX-1] = 0;
			ret = tcp_set_congestion_control(sk, name, false,
							 reinit);
		} else {
			struct tcp_sock *tp = tcp_sk(sk);

			if (optlen != sizeof(int))
				return -EINVAL;

			val = *((int *)optval);
			/* Only some options are supported */
			switch (optname) {
			case TCP_BPF_IW:
				if (val <= 0 || tp->data_segs_out > 0)
					ret = -EINVAL;
				else
					tp->snd_cwnd = val;
				break;
			case TCP_BPF_SNDCWND_CLAMP:
				if (val <= 0) {
					ret = -EINVAL;
				} else {
					tp->snd_cwnd_clamp = val;
					tp->snd_ssthresh = val;
				}
				break;
			default:
				ret = -EINVAL;
			}
		}
#endif
	} else {
		ret = -EINVAL;
	}
	return ret;
}