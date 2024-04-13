static inline __u32 xfrm_smark_get(__u32 mark, struct xfrm_state *x)
{
	struct xfrm_mark *m = &x->props.smark;

	return (m->v & m->m) | (mark & ~m->m);
}