xfrm_address_t *xfrm_flowi_daddr(const struct flowi *fl, unsigned short family)
{
	switch (family){
	case AF_INET:
		return (xfrm_address_t *)&fl->u.ip4.daddr;
	case AF_INET6:
		return (xfrm_address_t *)&fl->u.ip6.daddr;
	}
	return NULL;
}