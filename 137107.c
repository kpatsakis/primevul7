static int __bpf_redirect_no_mac(struct sk_buff *skb, struct net_device *dev,
				 u32 flags)
{
	/* skb->mac_len is not set on normal egress */
	unsigned int mlen = skb->network_header - skb->mac_header;

	__skb_pull(skb, mlen);

	/* At ingress, the mac header has already been pulled once.
	 * At egress, skb_pospull_rcsum has to be done in case that
	 * the skb is originated from ingress (i.e. a forwarded skb)
	 * to ensure that rcsum starts at net header.
	 */
	if (!skb_at_tc_ingress(skb))
		skb_postpull_rcsum(skb, skb_mac_header(skb), mlen);
	skb_pop_mac_header(skb);
	skb_reset_mac_len(skb);
	return flags & BPF_F_INGRESS ?
	       __bpf_rx_skb_no_mac(dev, skb) : __bpf_tx_skb(dev, skb);
}