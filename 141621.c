static inline int xfrm_mark_get(struct nlattr **attrs, struct xfrm_mark *m)
{
	if (attrs[XFRMA_MARK])
		memcpy(m, nla_data(attrs[XFRMA_MARK]), sizeof(struct xfrm_mark));
	else
		m->v = m->m = 0;

	return m->v & m->m;
}