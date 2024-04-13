static inline struct net *xs_net(struct xfrm_state *x)
{
	return read_pnet(&x->xs_net);
}