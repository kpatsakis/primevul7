static void xfrm6_tunnel_free_spi(struct net *net, xfrm_address_t *saddr)
{
	struct xfrm6_tunnel_net *xfrm6_tn = xfrm6_tunnel_pernet(net);
	struct xfrm6_tunnel_spi *x6spi;
	struct hlist_node *n;

	spin_lock_bh(&xfrm6_tunnel_spi_lock);

	hlist_for_each_entry_safe(x6spi, n,
				  &xfrm6_tn->spi_byaddr[xfrm6_tunnel_spi_hash_byaddr(saddr)],
				  list_byaddr)
	{
		if (xfrm6_addr_equal(&x6spi->addr, saddr)) {
			if (refcount_dec_and_test(&x6spi->refcnt)) {
				hlist_del_rcu(&x6spi->list_byaddr);
				hlist_del_rcu(&x6spi->list_byspi);
				call_rcu(&x6spi->rcu_head, x6spi_destroy_rcu);
				break;
			}
		}
	}
	spin_unlock_bh(&xfrm6_tunnel_spi_lock);
}