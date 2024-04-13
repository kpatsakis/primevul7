static inline bool xfrm_addr_equal(const xfrm_address_t *a,
				   const xfrm_address_t *b,
				   sa_family_t family)
{
	switch (family) {
	default:
	case AF_INET:
		return ((__force u32)a->a4 ^ (__force u32)b->a4) == 0;
	case AF_INET6:
		return xfrm6_addr_equal(a, b);
	}
}