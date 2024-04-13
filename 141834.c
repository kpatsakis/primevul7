static inline unsigned int xfrm6_tunnel_spi_hash_byaddr(const xfrm_address_t *addr)
{
	unsigned int h;

	h = ipv6_addr_hash((const struct in6_addr *)addr);
	h ^= h >> 16;
	h ^= h >> 8;
	h &= XFRM6_TUNNEL_SPI_BYADDR_HSIZE - 1;

	return h;
}