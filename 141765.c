xfrm_addr_any(const xfrm_address_t *addr, unsigned short family)
{
	switch (family) {
	case AF_INET:
		return addr->a4 == 0;
	case AF_INET6:
		return ipv6_addr_any(&addr->in6);
	}
	return 0;
}