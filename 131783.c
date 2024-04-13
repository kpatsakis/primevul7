int ieee80211_sta_ps_transition(struct ieee80211_sta *pubsta, bool start)
{
	struct sta_info *sta = container_of(pubsta, struct sta_info, sta);
	bool in_ps;

	WARN_ON(!ieee80211_hw_check(&sta->local->hw, AP_LINK_PS));

	/* Don't let the same PS state be set twice */
	in_ps = test_sta_flag(sta, WLAN_STA_PS_STA);
	if ((start && in_ps) || (!start && !in_ps))
		return -EINVAL;

	if (start)
		sta_ps_start(sta);
	else
		sta_ps_end(sta);

	return 0;
}