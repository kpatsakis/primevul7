BPF_CALL_1(__skb_get_pay_offset, struct sk_buff *, skb)
{
	return skb_get_poff(skb);
}