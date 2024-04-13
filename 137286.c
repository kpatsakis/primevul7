BPF_CALL_1(bpf_get_hash_recalc, struct sk_buff *, skb)
{
	/* If skb_clear_hash() was called due to mangling, we can
	 * trigger SW recalculation here. Later access to hash
	 * can then use the inline skb->hash via context directly
	 * instead of calling this helper again.
	 */
	return skb_get_hash(skb);
}