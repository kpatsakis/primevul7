static inline int __bpf_rx_skb(struct net_device *dev, struct sk_buff *skb)
{
	return dev_forward_skb(dev, skb);
}