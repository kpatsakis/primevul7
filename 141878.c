__xfrm6_state_addr_check(const struct xfrm_state *x,
			 const xfrm_address_t *daddr, const xfrm_address_t *saddr)
{
	if (ipv6_addr_equal((struct in6_addr *)daddr, (struct in6_addr *)&x->id.daddr) &&
	    (ipv6_addr_equal((struct in6_addr *)saddr, (struct in6_addr *)&x->props.saddr) ||
	     ipv6_addr_any((struct in6_addr *)saddr) ||
	     ipv6_addr_any((struct in6_addr *)&x->props.saddr)))
		return 1;
	return 0;
}