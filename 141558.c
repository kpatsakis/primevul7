static inline void xfrm_dst_destroy(struct xfrm_dst *xdst)
{
	xfrm_pols_put(xdst->pols, xdst->num_pols);
	dst_release(xdst->route);
	if (likely(xdst->u.dst.xfrm))
		xfrm_state_put(xdst->u.dst.xfrm);
}