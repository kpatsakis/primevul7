sock_set_v6only(int s)
{
#if defined(IPV6_V6ONLY) && !defined(__OpenBSD__)
	int on = 1;

	debug3("%s: set socket %d IPV6_V6ONLY", __func__, s);
	if (setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)) == -1)
		error("setsockopt IPV6_V6ONLY: %s", strerror(errno));
#endif
}