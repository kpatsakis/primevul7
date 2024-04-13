BPF_CALL_4(bpf_skb_adjust_room, struct sk_buff *, skb, s32, len_diff,
	   u32, mode, u64, flags)
{
	if (unlikely(flags))
		return -EINVAL;
	if (likely(mode == BPF_ADJ_ROOM_NET))
		return bpf_skb_adjust_net(skb, len_diff);

	return -ENOTSUPP;
}