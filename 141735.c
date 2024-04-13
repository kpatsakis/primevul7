static inline int xfrm_if_id_put(struct sk_buff *skb, __u32 if_id)
{
	int ret = 0;

	if (if_id)
		ret = nla_put_u32(skb, XFRMA_IF_ID, if_id);
	return ret;
}