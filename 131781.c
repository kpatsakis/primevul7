ieee80211_rx_h_defragment(struct ieee80211_rx_data *rx)
{
	struct ieee80211_hdr *hdr;
	u16 sc;
	__le16 fc;
	unsigned int frag, seq;
	struct ieee80211_fragment_entry *entry;
	struct sk_buff *skb;

	hdr = (struct ieee80211_hdr *)rx->skb->data;
	fc = hdr->frame_control;

	if (ieee80211_is_ctl(fc))
		return RX_CONTINUE;

	sc = le16_to_cpu(hdr->seq_ctrl);
	frag = sc & IEEE80211_SCTL_FRAG;

	if (is_multicast_ether_addr(hdr->addr1)) {
		I802_DEBUG_INC(rx->local->dot11MulticastReceivedFrameCount);
		goto out_no_led;
	}

	if (likely(!ieee80211_has_morefrags(fc) && frag == 0))
		goto out;

	I802_DEBUG_INC(rx->local->rx_handlers_fragments);

	if (skb_linearize(rx->skb))
		return RX_DROP_UNUSABLE;

	/*
	 *  skb_linearize() might change the skb->data and
	 *  previously cached variables (in this case, hdr) need to
	 *  be refreshed with the new data.
	 */
	hdr = (struct ieee80211_hdr *)rx->skb->data;
	seq = (sc & IEEE80211_SCTL_SEQ) >> 4;

	if (frag == 0) {
		/* This is the first fragment of a new frame. */
		entry = ieee80211_reassemble_add(rx->sdata, frag, seq,
						 rx->seqno_idx, &(rx->skb));
		if (rx->key &&
		    (rx->key->conf.cipher == WLAN_CIPHER_SUITE_CCMP ||
		     rx->key->conf.cipher == WLAN_CIPHER_SUITE_CCMP_256 ||
		     rx->key->conf.cipher == WLAN_CIPHER_SUITE_GCMP ||
		     rx->key->conf.cipher == WLAN_CIPHER_SUITE_GCMP_256) &&
		    ieee80211_has_protected(fc)) {
			int queue = rx->security_idx;

			/* Store CCMP/GCMP PN so that we can verify that the
			 * next fragment has a sequential PN value.
			 */
			entry->check_sequential_pn = true;
			memcpy(entry->last_pn,
			       rx->key->u.ccmp.rx_pn[queue],
			       IEEE80211_CCMP_PN_LEN);
			BUILD_BUG_ON(offsetof(struct ieee80211_key,
					      u.ccmp.rx_pn) !=
				     offsetof(struct ieee80211_key,
					      u.gcmp.rx_pn));
			BUILD_BUG_ON(sizeof(rx->key->u.ccmp.rx_pn[queue]) !=
				     sizeof(rx->key->u.gcmp.rx_pn[queue]));
			BUILD_BUG_ON(IEEE80211_CCMP_PN_LEN !=
				     IEEE80211_GCMP_PN_LEN);
		}
		return RX_QUEUED;
	}

	/* This is a fragment for a frame that should already be pending in
	 * fragment cache. Add this fragment to the end of the pending entry.
	 */
	entry = ieee80211_reassemble_find(rx->sdata, frag, seq,
					  rx->seqno_idx, hdr);
	if (!entry) {
		I802_DEBUG_INC(rx->local->rx_handlers_drop_defrag);
		return RX_DROP_MONITOR;
	}

	/* "The receiver shall discard MSDUs and MMPDUs whose constituent
	 *  MPDU PN values are not incrementing in steps of 1."
	 * see IEEE P802.11-REVmc/D5.0, 12.5.3.4.4, item d (for CCMP)
	 * and IEEE P802.11-REVmc/D5.0, 12.5.5.4.4, item d (for GCMP)
	 */
	if (entry->check_sequential_pn) {
		int i;
		u8 pn[IEEE80211_CCMP_PN_LEN], *rpn;
		int queue;

		if (!rx->key ||
		    (rx->key->conf.cipher != WLAN_CIPHER_SUITE_CCMP &&
		     rx->key->conf.cipher != WLAN_CIPHER_SUITE_CCMP_256 &&
		     rx->key->conf.cipher != WLAN_CIPHER_SUITE_GCMP &&
		     rx->key->conf.cipher != WLAN_CIPHER_SUITE_GCMP_256))
			return RX_DROP_UNUSABLE;
		memcpy(pn, entry->last_pn, IEEE80211_CCMP_PN_LEN);
		for (i = IEEE80211_CCMP_PN_LEN - 1; i >= 0; i--) {
			pn[i]++;
			if (pn[i])
				break;
		}
		queue = rx->security_idx;
		rpn = rx->key->u.ccmp.rx_pn[queue];
		if (memcmp(pn, rpn, IEEE80211_CCMP_PN_LEN))
			return RX_DROP_UNUSABLE;
		memcpy(entry->last_pn, pn, IEEE80211_CCMP_PN_LEN);
	}

	skb_pull(rx->skb, ieee80211_hdrlen(fc));
	__skb_queue_tail(&entry->skb_list, rx->skb);
	entry->last_frag = frag;
	entry->extra_len += rx->skb->len;
	if (ieee80211_has_morefrags(fc)) {
		rx->skb = NULL;
		return RX_QUEUED;
	}

	rx->skb = __skb_dequeue(&entry->skb_list);
	if (skb_tailroom(rx->skb) < entry->extra_len) {
		I802_DEBUG_INC(rx->local->rx_expand_skb_head_defrag);
		if (unlikely(pskb_expand_head(rx->skb, 0, entry->extra_len,
					      GFP_ATOMIC))) {
			I802_DEBUG_INC(rx->local->rx_handlers_drop_defrag);
			__skb_queue_purge(&entry->skb_list);
			return RX_DROP_UNUSABLE;
		}
	}
	while ((skb = __skb_dequeue(&entry->skb_list))) {
		skb_put_data(rx->skb, skb->data, skb->len);
		dev_kfree_skb(skb);
	}

 out:
	ieee80211_led_rx(rx->local);
 out_no_led:
	if (rx->sta)
		rx->sta->rx_stats.packets++;
	return RX_CONTINUE;
}