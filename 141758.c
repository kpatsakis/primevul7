static inline struct xfrm_algo *xfrm_algo_clone(struct xfrm_algo *orig)
{
	return kmemdup(orig, xfrm_alg_len(orig), GFP_KERNEL);
}