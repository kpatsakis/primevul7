static int bpf_skb_adjust_net(struct sk_buff *skb, s32 len_diff)
{
	bool trans_same = skb->transport_header == skb->network_header;
	u32 len_cur, len_diff_abs = abs(len_diff);
	u32 len_min = bpf_skb_net_base_len(skb);
	u32 len_max = __bpf_skb_max_len(skb);
	__be16 proto = skb->protocol;
	bool shrink = len_diff < 0;
	int ret;

	if (unlikely(len_diff_abs > 0xfffU))
		return -EFAULT;
	if (unlikely(proto != htons(ETH_P_IP) &&
		     proto != htons(ETH_P_IPV6)))
		return -ENOTSUPP;

	len_cur = skb->len - skb_network_offset(skb);
	if (skb_transport_header_was_set(skb) && !trans_same)
		len_cur = skb_network_header_len(skb);
	if ((shrink && (len_diff_abs >= len_cur ||
			len_cur - len_diff_abs < len_min)) ||
	    (!shrink && (skb->len + len_diff_abs > len_max &&
			 !skb_is_gso(skb))))
		return -ENOTSUPP;

	ret = shrink ? bpf_skb_net_shrink(skb, len_diff_abs) :
		       bpf_skb_net_grow(skb, len_diff_abs);

	bpf_compute_data_pointers(skb);
	return ret;
}