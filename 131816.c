static void __ieee80211_rx_handle_packet(struct ieee80211_hw *hw,
					 struct ieee80211_sta *pubsta,
					 struct sk_buff *skb,
					 struct napi_struct *napi)
{
	struct ieee80211_local *local = hw_to_local(hw);
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_hdr *hdr;
	__le16 fc;
	struct ieee80211_rx_data rx;
	struct ieee80211_sub_if_data *prev;
	struct rhlist_head *tmp;
	int err = 0;

	fc = ((struct ieee80211_hdr *)skb->data)->frame_control;
	memset(&rx, 0, sizeof(rx));
	rx.skb = skb;
	rx.local = local;
	rx.napi = napi;

	if (ieee80211_is_data(fc) || ieee80211_is_mgmt(fc))
		I802_DEBUG_INC(local->dot11ReceivedFragmentCount);

	if (ieee80211_is_mgmt(fc)) {
		/* drop frame if too short for header */
		if (skb->len < ieee80211_hdrlen(fc))
			err = -ENOBUFS;
		else
			err = skb_linearize(skb);
	} else {
		err = !pskb_may_pull(skb, ieee80211_hdrlen(fc));
	}

	if (err) {
		dev_kfree_skb(skb);
		return;
	}

	hdr = (struct ieee80211_hdr *)skb->data;
	ieee80211_parse_qos(&rx);
	ieee80211_verify_alignment(&rx);

	if (unlikely(ieee80211_is_probe_resp(hdr->frame_control) ||
		     ieee80211_is_beacon(hdr->frame_control)))
		ieee80211_scan_rx(local, skb);

	if (ieee80211_is_data(fc)) {
		struct sta_info *sta, *prev_sta;

		if (pubsta) {
			rx.sta = container_of(pubsta, struct sta_info, sta);
			rx.sdata = rx.sta->sdata;
			if (ieee80211_prepare_and_rx_handle(&rx, skb, true))
				return;
			goto out;
		}

		prev_sta = NULL;

		for_each_sta_info(local, hdr->addr2, sta, tmp) {
			if (!prev_sta) {
				prev_sta = sta;
				continue;
			}

			rx.sta = prev_sta;
			rx.sdata = prev_sta->sdata;
			ieee80211_prepare_and_rx_handle(&rx, skb, false);

			prev_sta = sta;
		}

		if (prev_sta) {
			rx.sta = prev_sta;
			rx.sdata = prev_sta->sdata;

			if (ieee80211_prepare_and_rx_handle(&rx, skb, true))
				return;
			goto out;
		}
	}

	prev = NULL;

	list_for_each_entry_rcu(sdata, &local->interfaces, list) {
		if (!ieee80211_sdata_running(sdata))
			continue;

		if (sdata->vif.type == NL80211_IFTYPE_MONITOR ||
		    sdata->vif.type == NL80211_IFTYPE_AP_VLAN)
			continue;

		/*
		 * frame is destined for this interface, but if it's
		 * not also for the previous one we handle that after
		 * the loop to avoid copying the SKB once too much
		 */

		if (!prev) {
			prev = sdata;
			continue;
		}

		rx.sta = sta_info_get_bss(prev, hdr->addr2);
		rx.sdata = prev;
		ieee80211_prepare_and_rx_handle(&rx, skb, false);

		prev = sdata;
	}

	if (prev) {
		rx.sta = sta_info_get_bss(prev, hdr->addr2);
		rx.sdata = prev;

		if (ieee80211_prepare_and_rx_handle(&rx, skb, true))
			return;
	}

 out:
	dev_kfree_skb(skb);
}