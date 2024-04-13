static void xfrm6_tunnel_destroy(struct xfrm_state *x)
{
	struct net *net = xs_net(x);

	xfrm6_tunnel_free_spi(net, (xfrm_address_t *)&x->props.saddr);
}