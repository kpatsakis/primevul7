static int __bpf_redirect_common(struct sk_buff *skb, struct net_device *dev,
				 u32 flags)
{
	/* Verify that a link layer header is carried */
	if (unlikely(skb->mac_header >= skb->network_header)) {
		kfree_skb(skb);
		return -ERANGE;
	}

	bpf_push_mac_rcsum(skb);
	return flags & BPF_F_INGRESS ?
	       __bpf_rx_skb(dev, skb) : __bpf_tx_skb(dev, skb);
}