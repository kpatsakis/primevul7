static inline unsigned int xfrm_alg_len(const struct xfrm_algo *alg)
{
	return sizeof(*alg) + ((alg->alg_key_len + 7) / 8);
}