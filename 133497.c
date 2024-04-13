 */
struct sk_buff *__skb_gso_segment(struct sk_buff *skb,
				  netdev_features_t features, bool tx_path)
{
	struct sk_buff *segs;

	if (unlikely(skb_needs_check(skb, tx_path))) {
		int err;

		/* We're going to init ->check field in TCP or UDP header */
		err = skb_cow_head(skb, 0);
		if (err < 0)
			return ERR_PTR(err);
	}

	/* Only report GSO partial support if it will enable us to
	 * support segmentation on this frame without needing additional
	 * work.
	 */
	if (features & NETIF_F_GSO_PARTIAL) {
		netdev_features_t partial_features = NETIF_F_GSO_ROBUST;
		struct net_device *dev = skb->dev;

		partial_features |= dev->features & dev->gso_partial_features;
		if (!skb_gso_ok(skb, features | partial_features))
			features &= ~NETIF_F_GSO_PARTIAL;
	}

	BUILD_BUG_ON(SKB_SGO_CB_OFFSET +
		     sizeof(*SKB_GSO_CB(skb)) > sizeof(skb->cb));

	SKB_GSO_CB(skb)->mac_offset = skb_headroom(skb);
	SKB_GSO_CB(skb)->encap_level = 0;

	skb_reset_mac_header(skb);
	skb_reset_mac_len(skb);

	segs = skb_mac_gso_segment(skb, features);

	if (unlikely(skb_needs_check(skb, tx_path) && !IS_ERR(segs)))
		skb_warn_bad_offload(skb);

	return segs;