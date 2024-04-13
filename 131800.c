static inline bool ieee80211_rx_reorder_ready(struct tid_ampdu_rx *tid_agg_rx,
					      int index)
{
	struct sk_buff_head *frames = &tid_agg_rx->reorder_buf[index];
	struct sk_buff *tail = skb_peek_tail(frames);
	struct ieee80211_rx_status *status;

	if (tid_agg_rx->reorder_buf_filtered & BIT_ULL(index))
		return true;

	if (!tail)
		return false;

	status = IEEE80211_SKB_RXCB(tail);
	if (status->flag & RX_FLAG_AMSDU_MORE)
		return false;

	return true;
}