static int bpf_skb_net_hdr_push(struct sk_buff *skb, u32 off, u32 len)
{
	bool trans_same = skb->transport_header == skb->network_header;
	int ret;

	/* There's no need for __skb_push()/__skb_pull() pair to
	 * get to the start of the mac header as we're guaranteed
	 * to always start from here under eBPF.
	 */
	ret = bpf_skb_generic_push(skb, off, len);
	if (likely(!ret)) {
		skb->mac_header -= len;
		skb->network_header -= len;
		if (trans_same)
			skb->transport_header = skb->network_header;
	}

	return ret;
}