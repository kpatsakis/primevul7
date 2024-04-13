static void remove_monitor_info(struct sk_buff *skb,
				unsigned int present_fcs_len,
				unsigned int rtap_space)
{
	if (present_fcs_len)
		__pskb_trim(skb, skb->len - present_fcs_len);
	__pskb_pull(skb, rtap_space);
}