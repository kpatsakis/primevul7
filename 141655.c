static inline void xfrm_dst_set_child(struct xfrm_dst *xdst, struct dst_entry *child)
{
	xdst->child = child;
}