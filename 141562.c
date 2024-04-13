static inline struct xfrm_offload *xfrm_offload(struct sk_buff *skb)
{
#ifdef CONFIG_XFRM
	struct sec_path *sp = skb_sec_path(skb);

	if (!sp || !sp->olen || sp->len != sp->olen)
		return NULL;

	return &sp->ovec[sp->olen - 1];
#else
	return NULL;
#endif
}