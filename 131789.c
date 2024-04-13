static bool ieee80211_sta_manage_reorder_buf(struct ieee80211_sub_if_data *sdata,
					     struct tid_ampdu_rx *tid_agg_rx,
					     struct sk_buff *skb,
					     struct sk_buff_head *frames)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	u16 sc = le16_to_cpu(hdr->seq_ctrl);
	u16 mpdu_seq_num = (sc & IEEE80211_SCTL_SEQ) >> 4;
	u16 head_seq_num, buf_size;
	int index;
	bool ret = true;

	spin_lock(&tid_agg_rx->reorder_lock);

	/*
	 * Offloaded BA sessions have no known starting sequence number so pick
	 * one from first Rxed frame for this tid after BA was started.
	 */
	if (unlikely(tid_agg_rx->auto_seq)) {
		tid_agg_rx->auto_seq = false;
		tid_agg_rx->ssn = mpdu_seq_num;
		tid_agg_rx->head_seq_num = mpdu_seq_num;
	}

	buf_size = tid_agg_rx->buf_size;
	head_seq_num = tid_agg_rx->head_seq_num;

	/*
	 * If the current MPDU's SN is smaller than the SSN, it shouldn't
	 * be reordered.
	 */
	if (unlikely(!tid_agg_rx->started)) {
		if (ieee80211_sn_less(mpdu_seq_num, head_seq_num)) {
			ret = false;
			goto out;
		}
		tid_agg_rx->started = true;
	}

	/* frame with out of date sequence number */
	if (ieee80211_sn_less(mpdu_seq_num, head_seq_num)) {
		dev_kfree_skb(skb);
		goto out;
	}

	/*
	 * If frame the sequence number exceeds our buffering window
	 * size release some previous frames to make room for this one.
	 */
	if (!ieee80211_sn_less(mpdu_seq_num, head_seq_num + buf_size)) {
		head_seq_num = ieee80211_sn_inc(
				ieee80211_sn_sub(mpdu_seq_num, buf_size));
		/* release stored frames up to new head to stack */
		ieee80211_release_reorder_frames(sdata, tid_agg_rx,
						 head_seq_num, frames);
	}

	/* Now the new frame is always in the range of the reordering buffer */

	index = mpdu_seq_num % tid_agg_rx->buf_size;

	/* check if we already stored this frame */
	if (ieee80211_rx_reorder_ready(tid_agg_rx, index)) {
		dev_kfree_skb(skb);
		goto out;
	}

	/*
	 * If the current MPDU is in the right order and nothing else
	 * is stored we can process it directly, no need to buffer it.
	 * If it is first but there's something stored, we may be able
	 * to release frames after this one.
	 */
	if (mpdu_seq_num == tid_agg_rx->head_seq_num &&
	    tid_agg_rx->stored_mpdu_num == 0) {
		if (!(status->flag & RX_FLAG_AMSDU_MORE))
			tid_agg_rx->head_seq_num =
				ieee80211_sn_inc(tid_agg_rx->head_seq_num);
		ret = false;
		goto out;
	}

	/* put the frame in the reordering buffer */
	__skb_queue_tail(&tid_agg_rx->reorder_buf[index], skb);
	if (!(status->flag & RX_FLAG_AMSDU_MORE)) {
		tid_agg_rx->reorder_time[index] = jiffies;
		tid_agg_rx->stored_mpdu_num++;
		ieee80211_sta_reorder_release(sdata, tid_agg_rx, frames);
	}

 out:
	spin_unlock(&tid_agg_rx->reorder_lock);
	return ret;
}