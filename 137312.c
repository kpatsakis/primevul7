static inline void bpf_pull_mac_rcsum(struct sk_buff *skb)
{
	if (skb_at_tc_ingress(skb))
		skb_postpull_rcsum(skb, skb_mac_header(skb), skb->mac_len);
}