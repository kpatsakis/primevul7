static inline int xfrm_mark_put(struct sk_buff *skb, const struct xfrm_mark *m)
{
	int ret = 0;

	if (m->m | m->v)
		ret = nla_put(skb, XFRMA_MARK, sizeof(struct xfrm_mark), m);
	return ret;
}