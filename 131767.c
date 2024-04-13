ieee80211_rx_h_sta_process(struct ieee80211_rx_data *rx)
{
	struct sta_info *sta = rx->sta;
	struct sk_buff *skb = rx->skb;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;
	int i;

	if (!sta)
		return RX_CONTINUE;

	/*
	 * Update last_rx only for IBSS packets which are for the current
	 * BSSID and for station already AUTHORIZED to avoid keeping the
	 * current IBSS network alive in cases where other STAs start
	 * using different BSSID. This will also give the station another
	 * chance to restart the authentication/authorization in case
	 * something went wrong the first time.
	 */
	if (rx->sdata->vif.type == NL80211_IFTYPE_ADHOC) {
		u8 *bssid = ieee80211_get_bssid(hdr, rx->skb->len,
						NL80211_IFTYPE_ADHOC);
		if (ether_addr_equal(bssid, rx->sdata->u.ibss.bssid) &&
		    test_sta_flag(sta, WLAN_STA_AUTHORIZED)) {
			sta->rx_stats.last_rx = jiffies;
			if (ieee80211_is_data(hdr->frame_control) &&
			    !is_multicast_ether_addr(hdr->addr1))
				sta->rx_stats.last_rate =
					sta_stats_encode_rate(status);
		}
	} else if (rx->sdata->vif.type == NL80211_IFTYPE_OCB) {
		sta->rx_stats.last_rx = jiffies;
	} else if (!is_multicast_ether_addr(hdr->addr1)) {
		/*
		 * Mesh beacons will update last_rx when if they are found to
		 * match the current local configuration when processed.
		 */
		sta->rx_stats.last_rx = jiffies;
		if (ieee80211_is_data(hdr->frame_control))
			sta->rx_stats.last_rate = sta_stats_encode_rate(status);
	}

	if (rx->sdata->vif.type == NL80211_IFTYPE_STATION)
		ieee80211_sta_rx_notify(rx->sdata, hdr);

	sta->rx_stats.fragments++;

	u64_stats_update_begin(&rx->sta->rx_stats.syncp);
	sta->rx_stats.bytes += rx->skb->len;
	u64_stats_update_end(&rx->sta->rx_stats.syncp);

	if (!(status->flag & RX_FLAG_NO_SIGNAL_VAL)) {
		sta->rx_stats.last_signal = status->signal;
		ewma_signal_add(&sta->rx_stats_avg.signal, -status->signal);
	}

	if (status->chains) {
		sta->rx_stats.chains = status->chains;
		for (i = 0; i < ARRAY_SIZE(status->chain_signal); i++) {
			int signal = status->chain_signal[i];

			if (!(status->chains & BIT(i)))
				continue;

			sta->rx_stats.chain_signal_last[i] = signal;
			ewma_signal_add(&sta->rx_stats_avg.chain_signal[i],
					-signal);
		}
	}

	/*
	 * Change STA power saving mode only at the end of a frame
	 * exchange sequence, and only for a data or management
	 * frame as specified in IEEE 802.11-2016 11.2.3.2
	 */
	if (!ieee80211_hw_check(&sta->local->hw, AP_LINK_PS) &&
	    !ieee80211_has_morefrags(hdr->frame_control) &&
	    !is_multicast_ether_addr(hdr->addr1) &&
	    (ieee80211_is_mgmt(hdr->frame_control) ||
	     ieee80211_is_data(hdr->frame_control)) &&
	    !(status->rx_flags & IEEE80211_RX_DEFERRED_RELEASE) &&
	    (rx->sdata->vif.type == NL80211_IFTYPE_AP ||
	     rx->sdata->vif.type == NL80211_IFTYPE_AP_VLAN)) {
		if (test_sta_flag(sta, WLAN_STA_PS_STA)) {
			if (!ieee80211_has_pm(hdr->frame_control))
				sta_ps_end(sta);
		} else {
			if (ieee80211_has_pm(hdr->frame_control))
				sta_ps_start(sta);
		}
	}

	/* mesh power save support */
	if (ieee80211_vif_is_mesh(&rx->sdata->vif))
		ieee80211_mps_rx_h_sta_process(sta, hdr);

	/*
	 * Drop (qos-)data::nullfunc frames silently, since they
	 * are used only to control station power saving mode.
	 */
	if (ieee80211_is_nullfunc(hdr->frame_control) ||
	    ieee80211_is_qos_nullfunc(hdr->frame_control)) {
		I802_DEBUG_INC(rx->local->rx_handlers_drop_nullfunc);

		/*
		 * If we receive a 4-addr nullfunc frame from a STA
		 * that was not moved to a 4-addr STA vlan yet send
		 * the event to userspace and for older hostapd drop
		 * the frame to the monitor interface.
		 */
		if (ieee80211_has_a4(hdr->frame_control) &&
		    (rx->sdata->vif.type == NL80211_IFTYPE_AP ||
		     (rx->sdata->vif.type == NL80211_IFTYPE_AP_VLAN &&
		      !rx->sdata->u.vlan.sta))) {
			if (!test_and_set_sta_flag(sta, WLAN_STA_4ADDR_EVENT))
				cfg80211_rx_unexpected_4addr_frame(
					rx->sdata->dev, sta->sta.addr,
					GFP_ATOMIC);
			return RX_DROP_MONITOR;
		}
		/*
		 * Update counter and free packet here to avoid
		 * counting this as a dropped packed.
		 */
		sta->rx_stats.packets++;
		dev_kfree_skb(rx->skb);
		return RX_QUEUED;
	}

	return RX_CONTINUE;
} /* ieee80211_rx_h_sta_process */