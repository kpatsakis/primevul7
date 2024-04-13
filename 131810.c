void ieee80211_check_fast_rx_iface(struct ieee80211_sub_if_data *sdata)
{
	struct ieee80211_local *local = sdata->local;

	mutex_lock(&local->sta_mtx);
	__ieee80211_check_fast_rx_iface(sdata);
	mutex_unlock(&local->sta_mtx);
}