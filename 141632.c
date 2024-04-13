__xfrm4_state_addr_check(const struct xfrm_state *x,
			 const xfrm_address_t *daddr, const xfrm_address_t *saddr)
{
	if (daddr->a4 == x->id.daddr.a4 &&
	    (saddr->a4 == x->props.saddr.a4 || !saddr->a4 || !x->props.saddr.a4))
		return 1;
	return 0;
}