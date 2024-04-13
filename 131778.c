static int ieee80211_get_mmie_keyidx(struct sk_buff *skb)
{
	struct ieee80211_mgmt *hdr = (struct ieee80211_mgmt *) skb->data;
	struct ieee80211_mmie *mmie;
	struct ieee80211_mmie_16 *mmie16;

	if (skb->len < 24 + sizeof(*mmie) || !is_multicast_ether_addr(hdr->da))
		return -1;

	if (!ieee80211_is_robust_mgmt_frame(skb))
		return -1; /* not a robust management frame */

	mmie = (struct ieee80211_mmie *)
		(skb->data + skb->len - sizeof(*mmie));
	if (mmie->element_id == WLAN_EID_MMIE &&
	    mmie->length == sizeof(*mmie) - 2)
		return le16_to_cpu(mmie->key_id);

	mmie16 = (struct ieee80211_mmie_16 *)
		(skb->data + skb->len - sizeof(*mmie16));
	if (skb->len >= 24 + sizeof(*mmie16) &&
	    mmie16->element_id == WLAN_EID_MMIE &&
	    mmie16->length == sizeof(*mmie16) - 2)
		return le16_to_cpu(mmie16->key_id);

	return -1;
}