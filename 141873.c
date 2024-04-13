static inline struct dst_entry *xfrm_dst_child(const struct dst_entry *dst)
{
#ifdef CONFIG_XFRM
	if (dst->xfrm) {
		struct xfrm_dst *xdst = (struct xfrm_dst *) dst;
		return xdst->child;
	}
#endif
	return NULL;
}