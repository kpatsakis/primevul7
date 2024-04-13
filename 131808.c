__ieee80211_rx_h_amsdu(struct ieee80211_rx_data *rx, u8 data_offset)
{
	struct net_device *dev = rx->sdata->dev;
	struct sk_buff *skb = rx->skb;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;
	__le16 fc = hdr->frame_control;
	struct sk_buff_head frame_list;
	struct ethhdr ethhdr;
	const u8 *check_da = ethhdr.h_dest, *check_sa = ethhdr.h_source;

	if (unlikely(ieee80211_has_a4(hdr->frame_control))) {
		check_da = NULL;
		check_sa = NULL;
	} else switch (rx->sdata->vif.type) {
		case NL80211_IFTYPE_AP:
		case NL80211_IFTYPE_AP_VLAN:
			check_da = NULL;
			break;
		case NL80211_IFTYPE_STATION:
			if (!rx->sta ||
			    !test_sta_flag(rx->sta, WLAN_STA_TDLS_PEER))
				check_sa = NULL;
			break;
		case NL80211_IFTYPE_MESH_POINT:
			check_sa = NULL;
			break;
		default:
			break;
	}

	skb->dev = dev;
	__skb_queue_head_init(&frame_list);

	if (ieee80211_data_to_8023_exthdr(skb, &ethhdr,
					  rx->sdata->vif.addr,
					  rx->sdata->vif.type,
					  data_offset))
		return RX_DROP_UNUSABLE;

	ieee80211_amsdu_to_8023s(skb, &frame_list, dev->dev_addr,
				 rx->sdata->vif.type,
				 rx->local->hw.extra_tx_headroom,
				 check_da, check_sa);

	while (!skb_queue_empty(&frame_list)) {
		rx->skb = __skb_dequeue(&frame_list);

		if (!ieee80211_frame_allowed(rx, fc)) {
			dev_kfree_skb(rx->skb);
			continue;
		}

		ieee80211_deliver_skb(rx);
	}

	return RX_QUEUED;
}