static ieee80211_rx_result ieee80211_rx_mesh_check(struct ieee80211_rx_data *rx)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)rx->skb->data;
	char *dev_addr = rx->sdata->vif.addr;

	if (ieee80211_is_data(hdr->frame_control)) {
		if (is_multicast_ether_addr(hdr->addr1)) {
			if (ieee80211_has_tods(hdr->frame_control) ||
			    !ieee80211_has_fromds(hdr->frame_control))
				return RX_DROP_MONITOR;
			if (ether_addr_equal(hdr->addr3, dev_addr))
				return RX_DROP_MONITOR;
		} else {
			if (!ieee80211_has_a4(hdr->frame_control))
				return RX_DROP_MONITOR;
			if (ether_addr_equal(hdr->addr4, dev_addr))
				return RX_DROP_MONITOR;
		}
	}

	/* If there is not an established peer link and this is not a peer link
	 * establisment frame, beacon or probe, drop the frame.
	 */

	if (!rx->sta || sta_plink_state(rx->sta) != NL80211_PLINK_ESTAB) {
		struct ieee80211_mgmt *mgmt;

		if (!ieee80211_is_mgmt(hdr->frame_control))
			return RX_DROP_MONITOR;

		if (ieee80211_is_action(hdr->frame_control)) {
			u8 category;

			/* make sure category field is present */
			if (rx->skb->len < IEEE80211_MIN_ACTION_SIZE)
				return RX_DROP_MONITOR;

			mgmt = (struct ieee80211_mgmt *)hdr;
			category = mgmt->u.action.category;
			if (category != WLAN_CATEGORY_MESH_ACTION &&
			    category != WLAN_CATEGORY_SELF_PROTECTED)
				return RX_DROP_MONITOR;
			return RX_CONTINUE;
		}

		if (ieee80211_is_probe_req(hdr->frame_control) ||
		    ieee80211_is_probe_resp(hdr->frame_control) ||
		    ieee80211_is_beacon(hdr->frame_control) ||
		    ieee80211_is_auth(hdr->frame_control))
			return RX_CONTINUE;

		return RX_DROP_MONITOR;
	}

	return RX_CONTINUE;
}