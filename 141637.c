static inline bool xfrm_dst_offload_ok(struct dst_entry *dst)
{
	struct xfrm_state *x = dst->xfrm;
	struct xfrm_dst *xdst;

	if (!x || !x->type_offload)
		return false;

	xdst = (struct xfrm_dst *) dst;
	if (!x->xso.offload_handle && !xdst->child->xfrm)
		return true;
	if (x->xso.offload_handle && (x->xso.dev == xfrm_dst_path(dst)->dev) &&
	    !xdst->child->xfrm)
		return true;

	return false;
}