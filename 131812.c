void ieee80211_clear_fast_rx(struct sta_info *sta)
{
	struct ieee80211_fast_rx *old;

	spin_lock_bh(&sta->lock);
	old = rcu_dereference_protected(sta->fast_rx, true);
	RCU_INIT_POINTER(sta->fast_rx, NULL);
	spin_unlock_bh(&sta->lock);

	if (old)
		kfree_rcu(old, rcu_head);
}