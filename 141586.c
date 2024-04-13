static inline struct xfrm_algo_auth *xfrm_algo_auth_clone(struct xfrm_algo_auth *orig)
{
	return kmemdup(orig, xfrm_alg_auth_len(orig), GFP_KERNEL);
}