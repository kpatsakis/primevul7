BPF_CALL_5(bpf_skb_store_bytes, struct sk_buff *, skb, u32, offset,
	   const void *, from, u32, len, u64, flags)
{
	void *ptr;

	if (unlikely(flags & ~(BPF_F_RECOMPUTE_CSUM | BPF_F_INVALIDATE_HASH)))
		return -EINVAL;
	if (unlikely(offset > 0xffff))
		return -EFAULT;
	if (unlikely(bpf_try_make_writable(skb, offset + len)))
		return -EFAULT;

	ptr = skb->data + offset;
	if (flags & BPF_F_RECOMPUTE_CSUM)
		__skb_postpull_rcsum(skb, ptr, len, offset);

	memcpy(ptr, from, len);

	if (flags & BPF_F_RECOMPUTE_CSUM)
		__skb_postpush_rcsum(skb, ptr, len, offset);
	if (flags & BPF_F_INVALIDATE_HASH)
		skb_clear_hash(skb);

	return 0;
}