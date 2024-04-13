static inline bool should_drop_frame(struct sk_buff *skb, int present_fcs_len,
				     unsigned int rtap_space)
{
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	struct ieee80211_hdr *hdr;

	hdr = (void *)(skb->data + rtap_space);

	if (status->flag & (RX_FLAG_FAILED_FCS_CRC |
			    RX_FLAG_FAILED_PLCP_CRC |
			    RX_FLAG_ONLY_MONITOR |
			    RX_FLAG_NO_PSDU))
		return true;

	if (unlikely(skb->len < 16 + present_fcs_len + rtap_space))
		return true;

	if (ieee80211_is_ctl(hdr->frame_control) &&
	    !ieee80211_is_pspoll(hdr->frame_control) &&
	    !ieee80211_is_back_req(hdr->frame_control))
		return true;

	return false;
}