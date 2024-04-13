ieee80211_rx_h_check_dup(struct ieee80211_rx_data *rx)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)rx->skb->data;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(rx->skb);

	if (status->flag & RX_FLAG_DUP_VALIDATED)
		return RX_CONTINUE;

	/*
	 * Drop duplicate 802.11 retransmissions
	 * (IEEE 802.11-2012: 9.3.2.10 "Duplicate detection and recovery")
	 */

	if (rx->skb->len < 24)
		return RX_CONTINUE;

	if (ieee80211_is_ctl(hdr->frame_control) ||
	    ieee80211_is_nullfunc(hdr->frame_control) ||
	    ieee80211_is_qos_nullfunc(hdr->frame_control) ||
	    is_multicast_ether_addr(hdr->addr1))
		return RX_CONTINUE;

	if (!rx->sta)
		return RX_CONTINUE;

	if (unlikely(ieee80211_has_retry(hdr->frame_control) &&
		     rx->sta->last_seq_ctrl[rx->seqno_idx] == hdr->seq_ctrl)) {
		I802_DEBUG_INC(rx->local->dot11FrameDuplicateCount);
		rx->sta->rx_stats.num_duplicates++;
		return RX_DROP_UNUSABLE;
	} else if (!(status->flag & RX_FLAG_AMSDU_MORE)) {
		rx->sta->last_seq_ctrl[rx->seqno_idx] = hdr->seq_ctrl;
	}

	return RX_CONTINUE;
}