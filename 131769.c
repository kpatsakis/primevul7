static void ieee80211_verify_alignment(struct ieee80211_rx_data *rx)
{
#ifdef CONFIG_MAC80211_VERBOSE_DEBUG
	WARN_ON_ONCE((unsigned long)rx->skb->data & 1);
#endif
}