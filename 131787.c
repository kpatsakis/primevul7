ieee80211_rx_h_amsdu(struct ieee80211_rx_data *rx)
{
	struct sk_buff *skb = rx->skb;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;
	__le16 fc = hdr->frame_control;

	if (!(status->rx_flags & IEEE80211_RX_AMSDU))
		return RX_CONTINUE;

	if (unlikely(!ieee80211_is_data(fc)))
		return RX_CONTINUE;

	if (unlikely(!ieee80211_is_data_present(fc)))
		return RX_DROP_MONITOR;

	if (unlikely(ieee80211_has_a4(hdr->frame_control))) {
		switch (rx->sdata->vif.type) {
		case NL80211_IFTYPE_AP_VLAN:
			if (!rx->sdata->u.vlan.sta)
				return RX_DROP_UNUSABLE;
			break;
		case NL80211_IFTYPE_STATION:
			if (!rx->sdata->u.mgd.use_4addr)
				return RX_DROP_UNUSABLE;
			break;
		default:
			return RX_DROP_UNUSABLE;
		}
	}

	if (is_multicast_ether_addr(hdr->addr1))
		return RX_DROP_UNUSABLE;

	return __ieee80211_rx_h_amsdu(rx, 0);
}