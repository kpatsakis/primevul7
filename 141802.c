static inline bool xfrm_state_hold_rcu(struct xfrm_state __rcu *x)
{
	return refcount_inc_not_zero(&x->refcnt);
}