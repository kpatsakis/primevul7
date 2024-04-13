static int bpf_skb_net_shrink(struct sk_buff *skb, u32 len_diff)
{
	u32 off = skb_mac_header_len(skb) + bpf_skb_net_base_len(skb);
	int ret;

	/* SCTP uses GSO_BY_FRAGS, thus cannot adjust it. */
	if (skb_is_gso(skb) && unlikely(skb_is_gso_sctp(skb)))
		return -ENOTSUPP;

	ret = skb_unclone(skb, GFP_ATOMIC);
	if (unlikely(ret < 0))
		return ret;

	ret = bpf_skb_net_hdr_pop(skb, off, len_diff);
	if (unlikely(ret < 0))
		return ret;

	if (skb_is_gso(skb)) {
		struct skb_shared_info *shinfo = skb_shinfo(skb);

		/* Due to header shrink, MSS can be upgraded. */
		skb_increase_gso_size(shinfo, len_diff);
		/* Header must be checked, and gso_segs recomputed. */
		shinfo->gso_type |= SKB_GSO_DODGY;
		shinfo->gso_segs = 0;
	}

	return 0;
}