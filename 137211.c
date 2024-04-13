BPF_CALL_3(bpf_bind, struct bpf_sock_addr_kern *, ctx, struct sockaddr *, addr,
	   int, addr_len)
{
#ifdef CONFIG_INET
	struct sock *sk = ctx->sk;
	int err;

	/* Binding to port can be expensive so it's prohibited in the helper.
	 * Only binding to IP is supported.
	 */
	err = -EINVAL;
	if (addr->sa_family == AF_INET) {
		if (addr_len < sizeof(struct sockaddr_in))
			return err;
		if (((struct sockaddr_in *)addr)->sin_port != htons(0))
			return err;
		return __inet_bind(sk, addr, addr_len, true, false);
#if IS_ENABLED(CONFIG_IPV6)
	} else if (addr->sa_family == AF_INET6) {
		if (addr_len < SIN6_LEN_RFC2133)
			return err;
		if (((struct sockaddr_in6 *)addr)->sin6_port != htons(0))
			return err;
		/* ipv6_bpf_stub cannot be NULL, since it's called from
		 * bpf_cgroup_inet6_connect hook and ipv6 is already loaded
		 */
		return ipv6_bpf_stub->inet6_bind(sk, addr, addr_len, true, false);
#endif /* CONFIG_IPV6 */
	}
#endif /* CONFIG_INET */

	return -EAFNOSUPPORT;
}