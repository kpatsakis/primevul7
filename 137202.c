static u32 __bpf_skb_max_len(const struct sk_buff *skb)
{
	return skb->dev->mtu + skb->dev->hard_header_len;
}