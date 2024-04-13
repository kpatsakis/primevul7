static inline void bpf_push_mac_rcsum(struct sk_buff *skb)
{
	if (skb_at_tc_ingress(skb))
		skb_postpush_rcsum(skb, skb_mac_header(skb), skb->mac_len);
}