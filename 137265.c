static int bpf_skb_proto_6_to_4(struct sk_buff *skb)
{
	const u32 len_diff = sizeof(struct ipv6hdr) - sizeof(struct iphdr);
	u32 off = skb_mac_header_len(skb);
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

		/* SKB_GSO_TCPV6 needs to be changed into
		 * SKB_GSO_TCPV4.
		 */
		if (shinfo->gso_type & SKB_GSO_TCPV6) {
			shinfo->gso_type &= ~SKB_GSO_TCPV6;
			shinfo->gso_type |=  SKB_GSO_TCPV4;
		}

		/* Due to IPv4 header, MSS can be upgraded. */
		skb_increase_gso_size(shinfo, len_diff);
		/* Header must be checked, and gso_segs recomputed. */
		shinfo->gso_type |= SKB_GSO_DODGY;
		shinfo->gso_segs = 0;
	}

	skb->protocol = htons(ETH_P_IP);
	skb_clear_hash(skb);

	return 0;
}