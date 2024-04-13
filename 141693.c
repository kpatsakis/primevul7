__xfrm6_state_addr_cmp(const struct xfrm_tmpl *tmpl, const struct xfrm_state *x)
{
	return	(!ipv6_addr_any((struct in6_addr*)&tmpl->saddr) &&
		 !ipv6_addr_equal((struct in6_addr *)&tmpl->saddr, (struct in6_addr*)&x->props.saddr));
}