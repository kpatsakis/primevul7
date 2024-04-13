BPF_CALL_1(bpf_set_hash_invalid, struct sk_buff *, skb)
{
	/* After all direct packet write, this can be used once for
	 * triggering a lazy recalc on next skb_get_hash() invocation.
	 */
	skb_clear_hash(skb);
	return 0;
}