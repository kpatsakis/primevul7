static inline int xfrm_af2proto(unsigned int family)
{
	switch(family) {
	case AF_INET:
		return IPPROTO_IPIP;
	case AF_INET6:
		return IPPROTO_IPV6;
	default:
		return 0;
	}
}