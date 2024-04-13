static inline int __bpf_rx_skb_no_mac(struct net_device *dev,
				      struct sk_buff *skb)
{
	int ret = ____dev_forward_skb(dev, skb);

	if (likely(!ret)) {
		skb->dev = dev;
		ret = netif_rx(skb);
	}

	return ret;
}