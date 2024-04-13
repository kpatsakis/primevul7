static inline void xfrm_state_put_sync(struct xfrm_state *x)
{
	if (refcount_dec_and_test(&x->refcnt))
		__xfrm_state_destroy(x, true);
}