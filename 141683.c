static inline void xfrm_pol_put(struct xfrm_policy *policy)
{
	if (refcount_dec_and_test(&policy->refcnt))
		xfrm_policy_destroy(policy);
}