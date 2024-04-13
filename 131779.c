ieee80211_rx_h_action(struct ieee80211_rx_data *rx)
{
	struct ieee80211_local *local = rx->local;
	struct ieee80211_sub_if_data *sdata = rx->sdata;
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *) rx->skb->data;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(rx->skb);
	int len = rx->skb->len;

	if (!ieee80211_is_action(mgmt->frame_control))
		return RX_CONTINUE;

	/* drop too small frames */
	if (len < IEEE80211_MIN_ACTION_SIZE)
		return RX_DROP_UNUSABLE;

	if (!rx->sta && mgmt->u.action.category != WLAN_CATEGORY_PUBLIC &&
	    mgmt->u.action.category != WLAN_CATEGORY_SELF_PROTECTED &&
	    mgmt->u.action.category != WLAN_CATEGORY_SPECTRUM_MGMT)
		return RX_DROP_UNUSABLE;

	switch (mgmt->u.action.category) {
	case WLAN_CATEGORY_HT:
		/* reject HT action frames from stations not supporting HT */
		if (!rx->sta->sta.ht_cap.ht_supported)
			goto invalid;

		if (sdata->vif.type != NL80211_IFTYPE_STATION &&
		    sdata->vif.type != NL80211_IFTYPE_MESH_POINT &&
		    sdata->vif.type != NL80211_IFTYPE_AP_VLAN &&
		    sdata->vif.type != NL80211_IFTYPE_AP &&
		    sdata->vif.type != NL80211_IFTYPE_ADHOC)
			break;

		/* verify action & smps_control/chanwidth are present */
		if (len < IEEE80211_MIN_ACTION_SIZE + 2)
			goto invalid;

		switch (mgmt->u.action.u.ht_smps.action) {
		case WLAN_HT_ACTION_SMPS: {
			struct ieee80211_supported_band *sband;
			enum ieee80211_smps_mode smps_mode;
			struct sta_opmode_info sta_opmode = {};

			/* convert to HT capability */
			switch (mgmt->u.action.u.ht_smps.smps_control) {
			case WLAN_HT_SMPS_CONTROL_DISABLED:
				smps_mode = IEEE80211_SMPS_OFF;
				break;
			case WLAN_HT_SMPS_CONTROL_STATIC:
				smps_mode = IEEE80211_SMPS_STATIC;
				break;
			case WLAN_HT_SMPS_CONTROL_DYNAMIC:
				smps_mode = IEEE80211_SMPS_DYNAMIC;
				break;
			default:
				goto invalid;
			}

			/* if no change do nothing */
			if (rx->sta->sta.smps_mode == smps_mode)
				goto handled;
			rx->sta->sta.smps_mode = smps_mode;
			sta_opmode.smps_mode =
				ieee80211_smps_mode_to_smps_mode(smps_mode);
			sta_opmode.changed = STA_OPMODE_SMPS_MODE_CHANGED;

			sband = rx->local->hw.wiphy->bands[status->band];

			rate_control_rate_update(local, sband, rx->sta,
						 IEEE80211_RC_SMPS_CHANGED);
			cfg80211_sta_opmode_change_notify(sdata->dev,
							  rx->sta->addr,
							  &sta_opmode,
							  GFP_ATOMIC);
			goto handled;
		}
		case WLAN_HT_ACTION_NOTIFY_CHANWIDTH: {
			struct ieee80211_supported_band *sband;
			u8 chanwidth = mgmt->u.action.u.ht_notify_cw.chanwidth;
			enum ieee80211_sta_rx_bandwidth max_bw, new_bw;
			struct sta_opmode_info sta_opmode = {};

			/* If it doesn't support 40 MHz it can't change ... */
			if (!(rx->sta->sta.ht_cap.cap &
					IEEE80211_HT_CAP_SUP_WIDTH_20_40))
				goto handled;

			if (chanwidth == IEEE80211_HT_CHANWIDTH_20MHZ)
				max_bw = IEEE80211_STA_RX_BW_20;
			else
				max_bw = ieee80211_sta_cap_rx_bw(rx->sta);

			/* set cur_max_bandwidth and recalc sta bw */
			rx->sta->cur_max_bandwidth = max_bw;
			new_bw = ieee80211_sta_cur_vht_bw(rx->sta);

			if (rx->sta->sta.bandwidth == new_bw)
				goto handled;

			rx->sta->sta.bandwidth = new_bw;
			sband = rx->local->hw.wiphy->bands[status->band];
			sta_opmode.bw =
				ieee80211_sta_rx_bw_to_chan_width(rx->sta);
			sta_opmode.changed = STA_OPMODE_MAX_BW_CHANGED;

			rate_control_rate_update(local, sband, rx->sta,
						 IEEE80211_RC_BW_CHANGED);
			cfg80211_sta_opmode_change_notify(sdata->dev,
							  rx->sta->addr,
							  &sta_opmode,
							  GFP_ATOMIC);
			goto handled;
		}
		default:
			goto invalid;
		}

		break;
	case WLAN_CATEGORY_PUBLIC:
		if (len < IEEE80211_MIN_ACTION_SIZE + 1)
			goto invalid;
		if (sdata->vif.type != NL80211_IFTYPE_STATION)
			break;
		if (!rx->sta)
			break;
		if (!ether_addr_equal(mgmt->bssid, sdata->u.mgd.bssid))
			break;
		if (mgmt->u.action.u.ext_chan_switch.action_code !=
				WLAN_PUB_ACTION_EXT_CHANSW_ANN)
			break;
		if (len < offsetof(struct ieee80211_mgmt,
				   u.action.u.ext_chan_switch.variable))
			goto invalid;
		goto queue;
	case WLAN_CATEGORY_VHT:
		if (sdata->vif.type != NL80211_IFTYPE_STATION &&
		    sdata->vif.type != NL80211_IFTYPE_MESH_POINT &&
		    sdata->vif.type != NL80211_IFTYPE_AP_VLAN &&
		    sdata->vif.type != NL80211_IFTYPE_AP &&
		    sdata->vif.type != NL80211_IFTYPE_ADHOC)
			break;

		/* verify action code is present */
		if (len < IEEE80211_MIN_ACTION_SIZE + 1)
			goto invalid;

		switch (mgmt->u.action.u.vht_opmode_notif.action_code) {
		case WLAN_VHT_ACTION_OPMODE_NOTIF: {
			/* verify opmode is present */
			if (len < IEEE80211_MIN_ACTION_SIZE + 2)
				goto invalid;
			goto queue;
		}
		case WLAN_VHT_ACTION_GROUPID_MGMT: {
			if (len < IEEE80211_MIN_ACTION_SIZE + 25)
				goto invalid;
			goto queue;
		}
		default:
			break;
		}
		break;
	case WLAN_CATEGORY_BACK:
		if (sdata->vif.type != NL80211_IFTYPE_STATION &&
		    sdata->vif.type != NL80211_IFTYPE_MESH_POINT &&
		    sdata->vif.type != NL80211_IFTYPE_AP_VLAN &&
		    sdata->vif.type != NL80211_IFTYPE_AP &&
		    sdata->vif.type != NL80211_IFTYPE_ADHOC)
			break;

		/* verify action_code is present */
		if (len < IEEE80211_MIN_ACTION_SIZE + 1)
			break;

		switch (mgmt->u.action.u.addba_req.action_code) {
		case WLAN_ACTION_ADDBA_REQ:
			if (len < (IEEE80211_MIN_ACTION_SIZE +
				   sizeof(mgmt->u.action.u.addba_req)))
				goto invalid;
			break;
		case WLAN_ACTION_ADDBA_RESP:
			if (len < (IEEE80211_MIN_ACTION_SIZE +
				   sizeof(mgmt->u.action.u.addba_resp)))
				goto invalid;
			break;
		case WLAN_ACTION_DELBA:
			if (len < (IEEE80211_MIN_ACTION_SIZE +
				   sizeof(mgmt->u.action.u.delba)))
				goto invalid;
			break;
		default:
			goto invalid;
		}

		goto queue;
	case WLAN_CATEGORY_SPECTRUM_MGMT:
		/* verify action_code is present */
		if (len < IEEE80211_MIN_ACTION_SIZE + 1)
			break;

		switch (mgmt->u.action.u.measurement.action_code) {
		case WLAN_ACTION_SPCT_MSR_REQ:
			if (status->band != NL80211_BAND_5GHZ)
				break;

			if (len < (IEEE80211_MIN_ACTION_SIZE +
				   sizeof(mgmt->u.action.u.measurement)))
				break;

			if (sdata->vif.type != NL80211_IFTYPE_STATION)
				break;

			ieee80211_process_measurement_req(sdata, mgmt, len);
			goto handled;
		case WLAN_ACTION_SPCT_CHL_SWITCH: {
			u8 *bssid;
			if (len < (IEEE80211_MIN_ACTION_SIZE +
				   sizeof(mgmt->u.action.u.chan_switch)))
				break;

			if (sdata->vif.type != NL80211_IFTYPE_STATION &&
			    sdata->vif.type != NL80211_IFTYPE_ADHOC &&
			    sdata->vif.type != NL80211_IFTYPE_MESH_POINT)
				break;

			if (sdata->vif.type == NL80211_IFTYPE_STATION)
				bssid = sdata->u.mgd.bssid;
			else if (sdata->vif.type == NL80211_IFTYPE_ADHOC)
				bssid = sdata->u.ibss.bssid;
			else if (sdata->vif.type == NL80211_IFTYPE_MESH_POINT)
				bssid = mgmt->sa;
			else
				break;

			if (!ether_addr_equal(mgmt->bssid, bssid))
				break;

			goto queue;
			}
		}
		break;
	case WLAN_CATEGORY_SA_QUERY:
		if (len < (IEEE80211_MIN_ACTION_SIZE +
			   sizeof(mgmt->u.action.u.sa_query)))
			break;

		switch (mgmt->u.action.u.sa_query.action) {
		case WLAN_ACTION_SA_QUERY_REQUEST:
			if (sdata->vif.type != NL80211_IFTYPE_STATION)
				break;
			ieee80211_process_sa_query_req(sdata, mgmt, len);
			goto handled;
		}
		break;
	case WLAN_CATEGORY_SELF_PROTECTED:
		if (len < (IEEE80211_MIN_ACTION_SIZE +
			   sizeof(mgmt->u.action.u.self_prot.action_code)))
			break;

		switch (mgmt->u.action.u.self_prot.action_code) {
		case WLAN_SP_MESH_PEERING_OPEN:
		case WLAN_SP_MESH_PEERING_CLOSE:
		case WLAN_SP_MESH_PEERING_CONFIRM:
			if (!ieee80211_vif_is_mesh(&sdata->vif))
				goto invalid;
			if (sdata->u.mesh.user_mpm)
				/* userspace handles this frame */
				break;
			goto queue;
		case WLAN_SP_MGK_INFORM:
		case WLAN_SP_MGK_ACK:
			if (!ieee80211_vif_is_mesh(&sdata->vif))
				goto invalid;
			break;
		}
		break;
	case WLAN_CATEGORY_MESH_ACTION:
		if (len < (IEEE80211_MIN_ACTION_SIZE +
			   sizeof(mgmt->u.action.u.mesh_action.action_code)))
			break;

		if (!ieee80211_vif_is_mesh(&sdata->vif))
			break;
		if (mesh_action_is_path_sel(mgmt) &&
		    !mesh_path_sel_is_hwmp(sdata))
			break;
		goto queue;
	}

	return RX_CONTINUE;

 invalid:
	status->rx_flags |= IEEE80211_RX_MALFORMED_ACTION_FRM;
	/* will return in the next handlers */
	return RX_CONTINUE;

 handled:
	if (rx->sta)
		rx->sta->rx_stats.packets++;
	dev_kfree_skb(rx->skb);
	return RX_QUEUED;

 queue:
	skb_queue_tail(&sdata->skb_queue, rx->skb);
	ieee80211_queue_work(&local->hw, &sdata->work);
	if (rx->sta)
		rx->sta->rx_stats.packets++;
	return RX_QUEUED;
}