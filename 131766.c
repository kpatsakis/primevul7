ieee80211_rx_h_data(struct ieee80211_rx_data *rx)
{
	struct ieee80211_sub_if_data *sdata = rx->sdata;
	struct ieee80211_local *local = rx->local;
	struct net_device *dev = sdata->dev;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)rx->skb->data;
	__le16 fc = hdr->frame_control;
	bool port_control;
	int err;

	if (unlikely(!ieee80211_is_data(hdr->frame_control)))
		return RX_CONTINUE;

	if (unlikely(!ieee80211_is_data_present(hdr->frame_control)))
		return RX_DROP_MONITOR;

	/*
	 * Send unexpected-4addr-frame event to hostapd. For older versions,
	 * also drop the frame to cooked monitor interfaces.
	 */
	if (ieee80211_has_a4(hdr->frame_control) &&
	    sdata->vif.type == NL80211_IFTYPE_AP) {
		if (rx->sta &&
		    !test_and_set_sta_flag(rx->sta, WLAN_STA_4ADDR_EVENT))
			cfg80211_rx_unexpected_4addr_frame(
				rx->sdata->dev, rx->sta->sta.addr, GFP_ATOMIC);
		return RX_DROP_MONITOR;
	}

	err = __ieee80211_data_to_8023(rx, &port_control);
	if (unlikely(err))
		return RX_DROP_UNUSABLE;

	if (!ieee80211_frame_allowed(rx, fc))
		return RX_DROP_MONITOR;

	/* directly handle TDLS channel switch requests/responses */
	if (unlikely(((struct ethhdr *)rx->skb->data)->h_proto ==
						cpu_to_be16(ETH_P_TDLS))) {
		struct ieee80211_tdls_data *tf = (void *)rx->skb->data;

		if (pskb_may_pull(rx->skb,
				  offsetof(struct ieee80211_tdls_data, u)) &&
		    tf->payload_type == WLAN_TDLS_SNAP_RFTYPE &&
		    tf->category == WLAN_CATEGORY_TDLS &&
		    (tf->action_code == WLAN_TDLS_CHANNEL_SWITCH_REQUEST ||
		     tf->action_code == WLAN_TDLS_CHANNEL_SWITCH_RESPONSE)) {
			skb_queue_tail(&local->skb_queue_tdls_chsw, rx->skb);
			schedule_work(&local->tdls_chsw_work);
			if (rx->sta)
				rx->sta->rx_stats.packets++;

			return RX_QUEUED;
		}
	}

	if (rx->sdata->vif.type == NL80211_IFTYPE_AP_VLAN &&
	    unlikely(port_control) && sdata->bss) {
		sdata = container_of(sdata->bss, struct ieee80211_sub_if_data,
				     u.ap);
		dev = sdata->dev;
		rx->sdata = sdata;
	}

	rx->skb->dev = dev;

	if (!ieee80211_hw_check(&local->hw, SUPPORTS_DYNAMIC_PS) &&
	    local->ps_sdata && local->hw.conf.dynamic_ps_timeout > 0 &&
	    !is_multicast_ether_addr(
		    ((struct ethhdr *)rx->skb->data)->h_dest) &&
	    (!local->scanning &&
	     !test_bit(SDATA_STATE_OFFCHANNEL, &sdata->state)))
		mod_timer(&local->dynamic_ps_timer, jiffies +
			  msecs_to_jiffies(local->hw.conf.dynamic_ps_timeout));

	ieee80211_deliver_skb(rx);

	return RX_QUEUED;
}