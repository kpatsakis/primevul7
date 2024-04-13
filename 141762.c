static inline struct xfrm6_tunnel_net *xfrm6_tunnel_pernet(struct net *net)
{
	return net_generic(net, xfrm6_tunnel_net_id);
}