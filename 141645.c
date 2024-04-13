xfrm_address_t *xfrm_flowi_saddr(const struct flowi *fl, unsigned short family)
{
	switch (family){
	case AF_INET:
		return (xfrm_address_t *)&fl->u.ip4.saddr;
	case AF_INET6:
		return (xfrm_address_t *)&fl->u.ip6.saddr;
	}
	return NULL;
}