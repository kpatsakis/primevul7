static inline struct dst_entry *xfrm_dst_path(const struct dst_entry *dst)
{
#ifdef CONFIG_XFRM
	if (dst->xfrm) {
		const struct xfrm_dst *xdst = (const struct xfrm_dst *) dst;

		return xdst->path;
	}
#endif
	return (struct dst_entry *) dst;
}