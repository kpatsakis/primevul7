ieee80211_deliver_skb(struct ieee80211_rx_data *rx)
{
	struct ieee80211_sub_if_data *sdata = rx->sdata;
	struct net_device *dev = sdata->dev;
	struct sk_buff *skb, *xmit_skb;
	struct ethhdr *ehdr = (struct ethhdr *) rx->skb->data;
	struct sta_info *dsta;

	skb = rx->skb;
	xmit_skb = NULL;

	ieee80211_rx_stats(dev, skb->len);

	if (rx->sta) {
		/* The seqno index has the same property as needed
		 * for the rx_msdu field, i.e. it is IEEE80211_NUM_TIDS
		 * for non-QoS-data frames. Here we know it's a data
		 * frame, so count MSDUs.
		 */
		u64_stats_update_begin(&rx->sta->rx_stats.syncp);
		rx->sta->rx_stats.msdu[rx->seqno_idx]++;
		u64_stats_update_end(&rx->sta->rx_stats.syncp);
	}

	if ((sdata->vif.type == NL80211_IFTYPE_AP ||
	     sdata->vif.type == NL80211_IFTYPE_AP_VLAN) &&
	    !(sdata->flags & IEEE80211_SDATA_DONT_BRIDGE_PACKETS) &&
	    (sdata->vif.type != NL80211_IFTYPE_AP_VLAN || !sdata->u.vlan.sta)) {
		if (is_multicast_ether_addr(ehdr->h_dest) &&
		    ieee80211_vif_get_num_mcast_if(sdata) != 0) {
			/*
			 * send multicast frames both to higher layers in
			 * local net stack and back to the wireless medium
			 */
			xmit_skb = skb_copy(skb, GFP_ATOMIC);
			if (!xmit_skb)
				net_info_ratelimited("%s: failed to clone multicast frame\n",
						    dev->name);
		} else if (!is_multicast_ether_addr(ehdr->h_dest) &&
			   !ether_addr_equal(ehdr->h_dest, ehdr->h_source)) {
			dsta = sta_info_get(sdata, ehdr->h_dest);
			if (dsta) {
				/*
				 * The destination station is associated to
				 * this AP (in this VLAN), so send the frame
				 * directly to it and do not pass it to local
				 * net stack.
				 */
				xmit_skb = skb;
				skb = NULL;
			}
		}
	}

#ifndef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
	if (skb) {
		/* 'align' will only take the values 0 or 2 here since all
		 * frames are required to be aligned to 2-byte boundaries
		 * when being passed to mac80211; the code here works just
		 * as well if that isn't true, but mac80211 assumes it can
		 * access fields as 2-byte aligned (e.g. for ether_addr_equal)
		 */
		int align;

		align = (unsigned long)(skb->data + sizeof(struct ethhdr)) & 3;
		if (align) {
			if (WARN_ON(skb_headroom(skb) < 3)) {
				dev_kfree_skb(skb);
				skb = NULL;
			} else {
				u8 *data = skb->data;
				size_t len = skb_headlen(skb);
				skb->data -= align;
				memmove(skb->data, data, len);
				skb_set_tail_pointer(skb, len);
			}
		}
	}
#endif

	if (skb) {
		skb->protocol = eth_type_trans(skb, dev);
		memset(skb->cb, 0, sizeof(skb->cb));

		ieee80211_deliver_skb_to_local_stack(skb, rx);
	}

	if (xmit_skb) {
		/*
		 * Send to wireless media and increase priority by 256 to
		 * keep the received priority instead of reclassifying
		 * the frame (see cfg80211_classify8021d).
		 */
		xmit_skb->priority += 256;
		xmit_skb->protocol = htons(ETH_P_802_3);
		skb_reset_network_header(xmit_skb);
		skb_reset_mac_header(xmit_skb);
		dev_queue_xmit(xmit_skb);
	}
}