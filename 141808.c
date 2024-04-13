static struct xfrm6_tunnel_spi *__xfrm6_tunnel_spi_lookup(struct net *net, const xfrm_address_t *saddr)
{
	struct xfrm6_tunnel_net *xfrm6_tn = xfrm6_tunnel_pernet(net);
	struct xfrm6_tunnel_spi *x6spi;

	hlist_for_each_entry_rcu(x6spi,
			     &xfrm6_tn->spi_byaddr[xfrm6_tunnel_spi_hash_byaddr(saddr)],
			     list_byaddr) {
		if (xfrm6_addr_equal(&x6spi->addr, saddr))
			return x6spi;
	}

	return NULL;
}