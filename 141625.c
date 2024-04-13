static inline bool xfrm6_addr_equal(const xfrm_address_t *a,
				    const xfrm_address_t *b)
{
	return ipv6_addr_equal((const struct in6_addr *)a,
			       (const struct in6_addr *)b);
}