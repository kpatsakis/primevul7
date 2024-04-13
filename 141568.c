__be32 xfrm6_tunnel_spi_lookup(struct net *net, const xfrm_address_t *saddr)
{
	struct xfrm6_tunnel_spi *x6spi;
	u32 spi;

	rcu_read_lock_bh();
	x6spi = __xfrm6_tunnel_spi_lookup(net, saddr);
	spi = x6spi ? x6spi->spi : 0;
	rcu_read_unlock_bh();
	return htonl(spi);
}