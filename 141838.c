static inline int xfrm_state_kern(const struct xfrm_state *x)
{
	return atomic_read(&x->tunnel_users);
}