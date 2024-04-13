static inline struct net *xp_net(const struct xfrm_policy *xp)
{
	return read_pnet(&xp->xp_net);
}