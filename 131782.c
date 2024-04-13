ieee80211_rx_h_check(struct ieee80211_rx_data *rx)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)rx->skb->data;

	/* Drop disallowed frame classes based on STA auth/assoc state;
	 * IEEE 802.11, Chap 5.5.
	 *
	 * mac80211 filters only based on association state, i.e. it drops
	 * Class 3 frames from not associated stations. hostapd sends
	 * deauth/disassoc frames when needed. In addition, hostapd is
	 * responsible for filtering on both auth and assoc states.
	 */

	if (ieee80211_vif_is_mesh(&rx->sdata->vif))
		return ieee80211_rx_mesh_check(rx);

	if (unlikely((ieee80211_is_data(hdr->frame_control) ||
		      ieee80211_is_pspoll(hdr->frame_control)) &&
		     rx->sdata->vif.type != NL80211_IFTYPE_ADHOC &&
		     rx->sdata->vif.type != NL80211_IFTYPE_WDS &&
		     rx->sdata->vif.type != NL80211_IFTYPE_OCB &&
		     (!rx->sta || !test_sta_flag(rx->sta, WLAN_STA_ASSOC)))) {
		/*
		 * accept port control frames from the AP even when it's not
		 * yet marked ASSOC to prevent a race where we don't set the
		 * assoc bit quickly enough before it sends the first frame
		 */
		if (rx->sta && rx->sdata->vif.type == NL80211_IFTYPE_STATION &&
		    ieee80211_is_data_present(hdr->frame_control)) {
			unsigned int hdrlen;
			__be16 ethertype;

			hdrlen = ieee80211_hdrlen(hdr->frame_control);

			if (rx->skb->len < hdrlen + 8)
				return RX_DROP_MONITOR;

			skb_copy_bits(rx->skb, hdrlen + 6, &ethertype, 2);
			if (ethertype == rx->sdata->control_port_protocol)
				return RX_CONTINUE;
		}

		if (rx->sdata->vif.type == NL80211_IFTYPE_AP &&
		    cfg80211_rx_spurious_frame(rx->sdata->dev,
					       hdr->addr2,
					       GFP_ATOMIC))
			return RX_DROP_UNUSABLE;

		return RX_DROP_MONITOR;
	}

	return RX_CONTINUE;
}