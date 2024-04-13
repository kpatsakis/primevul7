void xfrm_flowi_addr_get(const struct flowi *fl,
			 xfrm_address_t *saddr, xfrm_address_t *daddr,
			 unsigned short family)
{
	switch(family) {
	case AF_INET:
		memcpy(&saddr->a4, &fl->u.ip4.saddr, sizeof(saddr->a4));
		memcpy(&daddr->a4, &fl->u.ip4.daddr, sizeof(daddr->a4));
		break;
	case AF_INET6:
		saddr->in6 = fl->u.ip6.saddr;
		daddr->in6 = fl->u.ip6.daddr;
		break;
	}
}