static inline unsigned int xfrm_alg_auth_len(const struct xfrm_algo_auth *alg)
{
	return sizeof(*alg) + ((alg->alg_key_len + 7) / 8);
}