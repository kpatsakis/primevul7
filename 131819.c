static bool ieee80211_invoke_fast_rx(struct ieee80211_rx_data *rx,
				     struct ieee80211_fast_rx *fast_rx)
{
	struct sk_buff *skb = rx->skb;
	struct ieee80211_hdr *hdr = (void *)skb->data;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	struct sta_info *sta = rx->sta;
	int orig_len = skb->len;
	int hdrlen = ieee80211_hdrlen(hdr->frame_control);
	int snap_offs = hdrlen;
	struct {
		u8 snap[sizeof(rfc1042_header)];
		__be16 proto;
	} *payload __aligned(2);
	struct {
		u8 da[ETH_ALEN];
		u8 sa[ETH_ALEN];
	} addrs __aligned(2);
	struct ieee80211_sta_rx_stats *stats = &sta->rx_stats;

	if (fast_rx->uses_rss)
		stats = this_cpu_ptr(sta->pcpu_rx_stats);

	/* for parallel-rx, we need to have DUP_VALIDATED, otherwise we write
	 * to a common data structure; drivers can implement that per queue
	 * but we don't have that information in mac80211
	 */
	if (!(status->flag & RX_FLAG_DUP_VALIDATED))
		return false;

#define FAST_RX_CRYPT_FLAGS	(RX_FLAG_PN_VALIDATED | RX_FLAG_DECRYPTED)

	/* If using encryption, we also need to have:
	 *  - PN_VALIDATED: similar, but the implementation is tricky
	 *  - DECRYPTED: necessary for PN_VALIDATED
	 */
	if (fast_rx->key &&
	    (status->flag & FAST_RX_CRYPT_FLAGS) != FAST_RX_CRYPT_FLAGS)
		return false;

	if (unlikely(!ieee80211_is_data_present(hdr->frame_control)))
		return false;

	if (unlikely(ieee80211_is_frag(hdr)))
		return false;

	/* Since our interface address cannot be multicast, this
	 * implicitly also rejects multicast frames without the
	 * explicit check.
	 *
	 * We shouldn't get any *data* frames not addressed to us
	 * (AP mode will accept multicast *management* frames), but
	 * punting here will make it go through the full checks in
	 * ieee80211_accept_frame().
	 */
	if (!ether_addr_equal(fast_rx->vif_addr, hdr->addr1))
		return false;

	if ((hdr->frame_control & cpu_to_le16(IEEE80211_FCTL_FROMDS |
					      IEEE80211_FCTL_TODS)) !=
	    fast_rx->expected_ds_bits)
		return false;

	/* assign the key to drop unencrypted frames (later)
	 * and strip the IV/MIC if necessary
	 */
	if (fast_rx->key && !(status->flag & RX_FLAG_IV_STRIPPED)) {
		/* GCMP header length is the same */
		snap_offs += IEEE80211_CCMP_HDR_LEN;
	}

	if (!(status->rx_flags & IEEE80211_RX_AMSDU)) {
		if (!pskb_may_pull(skb, snap_offs + sizeof(*payload)))
			goto drop;

		payload = (void *)(skb->data + snap_offs);

		if (!ether_addr_equal(payload->snap, fast_rx->rfc1042_hdr))
			return false;

		/* Don't handle these here since they require special code.
		 * Accept AARP and IPX even though they should come with a
		 * bridge-tunnel header - but if we get them this way then
		 * there's little point in discarding them.
		 */
		if (unlikely(payload->proto == cpu_to_be16(ETH_P_TDLS) ||
			     payload->proto == fast_rx->control_port_protocol))
			return false;
	}

	/* after this point, don't punt to the slowpath! */

	if (rx->key && !(status->flag & RX_FLAG_MIC_STRIPPED) &&
	    pskb_trim(skb, skb->len - fast_rx->icv_len))
		goto drop;

	if (unlikely(fast_rx->sta_notify)) {
		ieee80211_sta_rx_notify(rx->sdata, hdr);
		fast_rx->sta_notify = false;
	}

	/* statistics part of ieee80211_rx_h_sta_process() */
	if (!(status->flag & RX_FLAG_NO_SIGNAL_VAL)) {
		stats->last_signal = status->signal;
		if (!fast_rx->uses_rss)
			ewma_signal_add(&sta->rx_stats_avg.signal,
					-status->signal);
	}

	if (status->chains) {
		int i;

		stats->chains = status->chains;
		for (i = 0; i < ARRAY_SIZE(status->chain_signal); i++) {
			int signal = status->chain_signal[i];

			if (!(status->chains & BIT(i)))
				continue;

			stats->chain_signal_last[i] = signal;
			if (!fast_rx->uses_rss)
				ewma_signal_add(&sta->rx_stats_avg.chain_signal[i],
						-signal);
		}
	}
	/* end of statistics */

	if (rx->key && !ieee80211_has_protected(hdr->frame_control))
		goto drop;

	if (status->rx_flags & IEEE80211_RX_AMSDU) {
		if (__ieee80211_rx_h_amsdu(rx, snap_offs - hdrlen) !=
		    RX_QUEUED)
			goto drop;

		return true;
	}

	stats->last_rx = jiffies;
	stats->last_rate = sta_stats_encode_rate(status);

	stats->fragments++;
	stats->packets++;

	/* do the header conversion - first grab the addresses */
	ether_addr_copy(addrs.da, skb->data + fast_rx->da_offs);
	ether_addr_copy(addrs.sa, skb->data + fast_rx->sa_offs);
	/* remove the SNAP but leave the ethertype */
	skb_pull(skb, snap_offs + sizeof(rfc1042_header));
	/* push the addresses in front */
	memcpy(skb_push(skb, sizeof(addrs)), &addrs, sizeof(addrs));

	skb->dev = fast_rx->dev;

	ieee80211_rx_stats(fast_rx->dev, skb->len);

	/* The seqno index has the same property as needed
	 * for the rx_msdu field, i.e. it is IEEE80211_NUM_TIDS
	 * for non-QoS-data frames. Here we know it's a data
	 * frame, so count MSDUs.
	 */
	u64_stats_update_begin(&stats->syncp);
	stats->msdu[rx->seqno_idx]++;
	stats->bytes += orig_len;
	u64_stats_update_end(&stats->syncp);

	if (fast_rx->internal_forward) {
		struct sk_buff *xmit_skb = NULL;
		if (is_multicast_ether_addr(addrs.da)) {
			xmit_skb = skb_copy(skb, GFP_ATOMIC);
		} else if (!ether_addr_equal(addrs.da, addrs.sa) &&
			   sta_info_get(rx->sdata, addrs.da)) {
			xmit_skb = skb;
			skb = NULL;
		}

		if (xmit_skb) {
			/*
			 * Send to wireless media and increase priority by 256
			 * to keep the received priority instead of
			 * reclassifying the frame (see cfg80211_classify8021d).
			 */
			xmit_skb->priority += 256;
			xmit_skb->protocol = htons(ETH_P_802_3);
			skb_reset_network_header(xmit_skb);
			skb_reset_mac_header(xmit_skb);
			dev_queue_xmit(xmit_skb);
		}

		if (!skb)
			return true;
	}

	/* deliver to local stack */
	skb->protocol = eth_type_trans(skb, fast_rx->dev);
	memset(skb->cb, 0, sizeof(skb->cb));
	if (rx->napi)
		napi_gro_receive(rx->napi, skb);
	else
		netif_receive_skb(skb);

	return true;
 drop:
	dev_kfree_skb(skb);
	stats->dropped++;
	return true;
}