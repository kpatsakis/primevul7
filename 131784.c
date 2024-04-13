void ieee80211_mark_rx_ba_filtered_frames(struct ieee80211_sta *pubsta, u8 tid,
					  u16 ssn, u64 filtered,
					  u16 received_mpdus)
{
	struct sta_info *sta;
	struct tid_ampdu_rx *tid_agg_rx;
	struct sk_buff_head frames;
	struct ieee80211_rx_data rx = {
		/* This is OK -- must be QoS data frame */
		.security_idx = tid,
		.seqno_idx = tid,
	};
	int i, diff;

	if (WARN_ON(!pubsta || tid >= IEEE80211_NUM_TIDS))
		return;

	__skb_queue_head_init(&frames);

	sta = container_of(pubsta, struct sta_info, sta);

	rx.sta = sta;
	rx.sdata = sta->sdata;
	rx.local = sta->local;

	rcu_read_lock();
	tid_agg_rx = rcu_dereference(sta->ampdu_mlme.tid_rx[tid]);
	if (!tid_agg_rx)
		goto out;

	spin_lock_bh(&tid_agg_rx->reorder_lock);

	if (received_mpdus >= IEEE80211_SN_MODULO >> 1) {
		int release;

		/* release all frames in the reorder buffer */
		release = (tid_agg_rx->head_seq_num + tid_agg_rx->buf_size) %
			   IEEE80211_SN_MODULO;
		ieee80211_release_reorder_frames(sta->sdata, tid_agg_rx,
						 release, &frames);
		/* update ssn to match received ssn */
		tid_agg_rx->head_seq_num = ssn;
	} else {
		ieee80211_release_reorder_frames(sta->sdata, tid_agg_rx, ssn,
						 &frames);
	}

	/* handle the case that received ssn is behind the mac ssn.
	 * it can be tid_agg_rx->buf_size behind and still be valid */
	diff = (tid_agg_rx->head_seq_num - ssn) & IEEE80211_SN_MASK;
	if (diff >= tid_agg_rx->buf_size) {
		tid_agg_rx->reorder_buf_filtered = 0;
		goto release;
	}
	filtered = filtered >> diff;
	ssn += diff;

	/* update bitmap */
	for (i = 0; i < tid_agg_rx->buf_size; i++) {
		int index = (ssn + i) % tid_agg_rx->buf_size;

		tid_agg_rx->reorder_buf_filtered &= ~BIT_ULL(index);
		if (filtered & BIT_ULL(i))
			tid_agg_rx->reorder_buf_filtered |= BIT_ULL(index);
	}

	/* now process also frames that the filter marking released */
	ieee80211_sta_reorder_release(sta->sdata, tid_agg_rx, &frames);

release:
	spin_unlock_bh(&tid_agg_rx->reorder_lock);

	ieee80211_rx_handlers(&rx, &frames);

 out:
	rcu_read_unlock();
}