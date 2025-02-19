static void __exit xfrm6_tunnel_fini(void)
{
	xfrm6_tunnel_deregister(&xfrm46_tunnel_handler, AF_INET);
	xfrm6_tunnel_deregister(&xfrm6_tunnel_handler, AF_INET6);
	xfrm_unregister_type(&xfrm6_tunnel_type, AF_INET6);
	unregister_pernet_subsys(&xfrm6_tunnel_net_ops);
	/* Someone maybe has gotten the xfrm6_tunnel_spi.
	 * So need to wait it.
	 */
	rcu_barrier();
	kmem_cache_destroy(xfrm6_tunnel_spi_kmem);
}