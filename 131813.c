static void ieee80211_deliver_skb_to_local_stack(struct sk_buff *skb,
						 struct ieee80211_rx_data *rx)
{
	struct ieee80211_sub_if_data *sdata = rx->sdata;
	struct net_device *dev = sdata->dev;

	if (unlikely((skb->protocol == sdata->control_port_protocol ||
		      skb->protocol == cpu_to_be16(ETH_P_PREAUTH)) &&
		     sdata->control_port_over_nl80211)) {
		struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
		bool noencrypt = status->flag & RX_FLAG_DECRYPTED;

		cfg80211_rx_control_port(dev, skb, noencrypt);
		dev_kfree_skb(skb);
	} else {
		/* deliver to local stack */
		if (rx->napi)
			napi_gro_receive(rx->napi, skb);
		else
			netif_receive_skb(skb);
	}
}