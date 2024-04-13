BPF_CALL_2(bpf_set_hash, struct sk_buff *, skb, u32, hash)
{
	/* Set user specified hash as L4(+), so that it gets returned
	 * on skb_get_hash() call unless BPF prog later on triggers a
	 * skb_clear_hash().
	 */
	__skb_set_sw_hash(skb, hash, true);
	return 0;
}