static int esp_output_tail_tcp(struct xfrm_state *x, struct sk_buff *skb)
{
	kfree_skb(skb);

	return -EOPNOTSUPP;
}