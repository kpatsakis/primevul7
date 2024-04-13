void ieee80211_sta_pspoll(struct ieee80211_sta *pubsta)
{
	struct sta_info *sta = container_of(pubsta, struct sta_info, sta);

	if (test_sta_flag(sta, WLAN_STA_SP))
		return;

	if (!test_sta_flag(sta, WLAN_STA_PS_DRIVER))
		ieee80211_sta_ps_deliver_poll_response(sta);
	else
		set_sta_flag(sta, WLAN_STA_PSPOLL);
}