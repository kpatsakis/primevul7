void ieee80211_sta_uapsd_trigger(struct ieee80211_sta *pubsta, u8 tid)
{
	struct sta_info *sta = container_of(pubsta, struct sta_info, sta);
	int ac = ieee80211_ac_from_tid(tid);

	/*
	 * If this AC is not trigger-enabled do nothing unless the
	 * driver is calling us after it already checked.
	 *
	 * NB: This could/should check a separate bitmap of trigger-
	 * enabled queues, but for now we only implement uAPSD w/o
	 * TSPEC changes to the ACs, so they're always the same.
	 */
	if (!(sta->sta.uapsd_queues & ieee80211_ac_to_qos_mask[ac]) &&
	    tid != IEEE80211_NUM_TIDS)
		return;

	/* if we are in a service period, do nothing */
	if (test_sta_flag(sta, WLAN_STA_SP))
		return;

	if (!test_sta_flag(sta, WLAN_STA_PS_DRIVER))
		ieee80211_sta_ps_deliver_uapsd(sta);
	else
		set_sta_flag(sta, WLAN_STA_UAPSD);
}