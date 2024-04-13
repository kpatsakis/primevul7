static u32 bpf_skb_net_base_len(const struct sk_buff *skb)
{
	switch (skb->protocol) {
	case htons(ETH_P_IP):
		return sizeof(struct iphdr);
	case htons(ETH_P_IPV6):
		return sizeof(struct ipv6hdr);
	default:
		return ~0U;
	}
}