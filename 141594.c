static inline void __xfrm_state_put(struct xfrm_state *x)
{
	refcount_dec(&x->refcnt);
}