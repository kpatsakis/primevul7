BPF_CALL_4(bpf_sk_redirect_map, struct sk_buff *, skb,
	   struct bpf_map *, map, u32, key, u64, flags)
{
	struct tcp_skb_cb *tcb = TCP_SKB_CB(skb);

	/* If user passes invalid input drop the packet. */
	if (unlikely(flags & ~(BPF_F_INGRESS)))
		return SK_DROP;

	tcb->bpf.key = key;
	tcb->bpf.flags = flags;
	tcb->bpf.map = map;

	return SK_PASS;
}