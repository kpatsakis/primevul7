static inline struct xfrm_algo_aead *xfrm_algo_aead_clone(struct xfrm_algo_aead *orig)
{
	return kmemdup(orig, aead_len(orig), GFP_KERNEL);
}