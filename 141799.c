static void x6spi_destroy_rcu(struct rcu_head *head)
{
	kmem_cache_free(xfrm6_tunnel_spi_kmem,
			container_of(head, struct xfrm6_tunnel_spi, rcu_head));
}