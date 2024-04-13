static void sta_ps_start(struct sta_info *sta)
{
	struct ieee80211_sub_if_data *sdata = sta->sdata;
	struct ieee80211_local *local = sdata->local;
	struct ps_data *ps;
	int tid;

	if (sta->sdata->vif.type == NL80211_IFTYPE_AP ||
	    sta->sdata->vif.type == NL80211_IFTYPE_AP_VLAN)
		ps = &sdata->bss->ps;
	else
		return;

	atomic_inc(&ps->num_sta_ps);
	set_sta_flag(sta, WLAN_STA_PS_STA);
	if (!ieee80211_hw_check(&local->hw, AP_LINK_PS))
		drv_sta_notify(local, sdata, STA_NOTIFY_SLEEP, &sta->sta);
	ps_dbg(sdata, "STA %pM aid %d enters power save mode\n",
	       sta->sta.addr, sta->sta.aid);

	ieee80211_clear_fast_xmit(sta);

	if (!sta->sta.txq[0])
		return;

	for (tid = 0; tid < IEEE80211_NUM_TIDS; tid++) {
		struct ieee80211_txq *txq = sta->sta.txq[tid];
		struct txq_info *txqi = to_txq_info(txq);

		spin_lock(&local->active_txq_lock[txq->ac]);
		if (!list_empty(&txqi->schedule_order))
			list_del_init(&txqi->schedule_order);
		spin_unlock(&local->active_txq_lock[txq->ac]);

		if (txq_has_queue(txq))
			set_bit(tid, &sta->txq_buffered_tids);
		else
			clear_bit(tid, &sta->txq_buffered_tids);
	}
}