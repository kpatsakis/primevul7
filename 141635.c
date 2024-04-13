xfrm_state_addr_flow_check(const struct xfrm_state *x, const struct flowi *fl,
			   unsigned short family)
{
	switch (family) {
	case AF_INET:
		return __xfrm4_state_addr_check(x,
						(const xfrm_address_t *)&fl->u.ip4.daddr,
						(const xfrm_address_t *)&fl->u.ip4.saddr);
	case AF_INET6:
		return __xfrm6_state_addr_check(x,
						(const xfrm_address_t *)&fl->u.ip6.daddr,
						(const xfrm_address_t *)&fl->u.ip6.saddr);
	}
	return 0;
}