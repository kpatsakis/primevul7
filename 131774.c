ieee80211_make_monitor_skb(struct ieee80211_local *local,
			   struct sk_buff **origskb,
			   struct ieee80211_rate *rate,
			   int rtap_space, bool use_origskb)
{
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(*origskb);
	int rt_hdrlen, needed_headroom;
	struct sk_buff *skb;

	/* room for the radiotap header based on driver features */
	rt_hdrlen = ieee80211_rx_radiotap_hdrlen(local, status, *origskb);
	needed_headroom = rt_hdrlen - rtap_space;

	if (use_origskb) {
		/* only need to expand headroom if necessary */
		skb = *origskb;
		*origskb = NULL;

		/*
		 * This shouldn't trigger often because most devices have an
		 * RX header they pull before we get here, and that should
		 * be big enough for our radiotap information. We should
		 * probably export the length to drivers so that we can have
		 * them allocate enough headroom to start with.
		 */
		if (skb_headroom(skb) < needed_headroom &&
		    pskb_expand_head(skb, needed_headroom, 0, GFP_ATOMIC)) {
			dev_kfree_skb(skb);
			return NULL;
		}
	} else {
		/*
		 * Need to make a copy and possibly remove radiotap header
		 * and FCS from the original.
		 */
		skb = skb_copy_expand(*origskb, needed_headroom, 0, GFP_ATOMIC);

		if (!skb)
			return NULL;
	}

	/* prepend radiotap information */
	ieee80211_add_rx_radiotap_header(local, skb, rate, rt_hdrlen, true);

	skb_reset_mac_header(skb);
	skb->ip_summed = CHECKSUM_UNNECESSARY;
	skb->pkt_type = PACKET_OTHERHOST;
	skb->protocol = htons(ETH_P_802_2);

	return skb;
}