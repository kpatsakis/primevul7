BPF_CALL_3(bpf_skb_change_proto, struct sk_buff *, skb, __be16, proto,
	   u64, flags)
{
	int ret;

	if (unlikely(flags))
		return -EINVAL;

	/* General idea is that this helper does the basic groundwork
	 * needed for changing the protocol, and eBPF program fills the
	 * rest through bpf_skb_store_bytes(), bpf_lX_csum_replace()
	 * and other helpers, rather than passing a raw buffer here.
	 *
	 * The rationale is to keep this minimal and without a need to
	 * deal with raw packet data. F.e. even if we would pass buffers
	 * here, the program still needs to call the bpf_lX_csum_replace()
	 * helpers anyway. Plus, this way we keep also separation of
	 * concerns, since f.e. bpf_skb_store_bytes() should only take
	 * care of stores.
	 *
	 * Currently, additional options and extension header space are
	 * not supported, but flags register is reserved so we can adapt
	 * that. For offloads, we mark packet as dodgy, so that headers
	 * need to be verified first.
	 */
	ret = bpf_skb_proto_xlat(skb, proto);
	bpf_compute_data_pointers(skb);
	return ret;
}