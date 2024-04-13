BPF_CALL_4(bpf_msg_redirect_map, struct sk_msg_buff *, msg,
	   struct bpf_map *, map, u32, key, u64, flags)
{
	/* If user passes invalid input drop the packet. */
	if (unlikely(flags & ~(BPF_F_INGRESS)))
		return SK_DROP;

	msg->key = key;
	msg->flags = flags;
	msg->map = map;

	return SK_PASS;
}