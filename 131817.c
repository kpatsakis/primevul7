static void ieee80211_release_reorder_frame(struct ieee80211_sub_if_data *sdata,
					    struct tid_ampdu_rx *tid_agg_rx,
					    int index,
					    struct sk_buff_head *frames)
{
	struct sk_buff_head *skb_list = &tid_agg_rx->reorder_buf[index];
	struct sk_buff *skb;
	struct ieee80211_rx_status *status;

	lockdep_assert_held(&tid_agg_rx->reorder_lock);

	if (skb_queue_empty(skb_list))
		goto no_frame;

	if (!ieee80211_rx_reorder_ready(tid_agg_rx, index)) {
		__skb_queue_purge(skb_list);
		goto no_frame;
	}

	/* release frames from the reorder ring buffer */
	tid_agg_rx->stored_mpdu_num--;
	while ((skb = __skb_dequeue(skb_list))) {
		status = IEEE80211_SKB_RXCB(skb);
		status->rx_flags |= IEEE80211_RX_DEFERRED_RELEASE;
		__skb_queue_tail(frames, skb);
	}

no_frame:
	tid_agg_rx->reorder_buf_filtered &= ~BIT_ULL(index);
	tid_agg_rx->head_seq_num = ieee80211_sn_inc(tid_agg_rx->head_seq_num);
}