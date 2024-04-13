static void ieee80211_handle_mu_mimo_mon(struct ieee80211_sub_if_data *sdata,
					 struct sk_buff *skb,
					 int rtap_space)
{
	struct {
		struct ieee80211_hdr_3addr hdr;
		u8 category;
		u8 action_code;
	} __packed __aligned(2) action;

	if (!sdata)
		return;

	BUILD_BUG_ON(sizeof(action) != IEEE80211_MIN_ACTION_SIZE + 1);

	if (skb->len < rtap_space + sizeof(action) +
		       VHT_MUMIMO_GROUPS_DATA_LEN)
		return;

	if (!is_valid_ether_addr(sdata->u.mntr.mu_follow_addr))
		return;

	skb_copy_bits(skb, rtap_space, &action, sizeof(action));

	if (!ieee80211_is_action(action.hdr.frame_control))
		return;

	if (action.category != WLAN_CATEGORY_VHT)
		return;

	if (action.action_code != WLAN_VHT_ACTION_GROUPID_MGMT)
		return;

	if (!ether_addr_equal(action.hdr.addr1, sdata->u.mntr.mu_follow_addr))
		return;

	skb = skb_copy(skb, GFP_ATOMIC);
	if (!skb)
		return;

	skb_queue_tail(&sdata->skb_queue, skb);
	ieee80211_queue_work(&sdata->local->hw, &sdata->work);
}