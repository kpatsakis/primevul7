BPF_CALL_2(bpf_skb_change_type, struct sk_buff *, skb, u32, pkt_type)
{
	/* We only allow a restricted subset to be changed for now. */
	if (unlikely(!skb_pkt_type_ok(skb->pkt_type) ||
		     !skb_pkt_type_ok(pkt_type)))
		return -EINVAL;

	skb->pkt_type = pkt_type;
	return 0;
}