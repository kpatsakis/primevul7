static inline void xfrm_state_hold(struct xfrm_state *x)
{
	refcount_inc(&x->refcnt);
}