static inline void xfrm_pols_put(struct xfrm_policy **pols, int npols)
{
	int i;
	for (i = npols - 1; i >= 0; --i)
		xfrm_pol_put(pols[i]);
}