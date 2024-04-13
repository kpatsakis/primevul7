static int __xfrm6_tunnel_spi_check(struct net *net, u32 spi)
{
	struct xfrm6_tunnel_net *xfrm6_tn = xfrm6_tunnel_pernet(net);
	struct xfrm6_tunnel_spi *x6spi;
	int index = xfrm6_tunnel_spi_hash_byspi(spi);

	hlist_for_each_entry(x6spi,
			     &xfrm6_tn->spi_byspi[index],
			     list_byspi) {
		if (x6spi->spi == spi)
			return -1;
	}
	return index;
}