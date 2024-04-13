
int skb_crc32c_csum_help(struct sk_buff *skb)
{
	__le32 crc32c_csum;
	int ret = 0, offset, start;

	if (skb->ip_summed != CHECKSUM_PARTIAL)
		goto out;

	if (unlikely(skb_is_gso(skb)))
		goto out;

	/* Before computing a checksum, we should make sure no frag could
	 * be modified by an external entity : checksum could be wrong.
	 */
	if (unlikely(skb_has_shared_frag(skb))) {
		ret = __skb_linearize(skb);
		if (ret)
			goto out;
	}
	start = skb_checksum_start_offset(skb);
	offset = start + offsetof(struct sctphdr, checksum);
	if (WARN_ON_ONCE(offset >= skb_headlen(skb))) {
		ret = -EINVAL;
		goto out;
	}
	if (skb_cloned(skb) &&
	    !skb_clone_writable(skb, offset + sizeof(__le32))) {
		ret = pskb_expand_head(skb, 0, 0, GFP_ATOMIC);
		if (ret)
			goto out;
	}
	crc32c_csum = cpu_to_le32(~__skb_checksum(skb, start,
						  skb->len - start, ~(__u32)0,
						  crc32c_csum_stub));
	*(__le32 *)(skb->data + offset) = crc32c_csum;
	skb->ip_summed = CHECKSUM_NONE;
	skb->csum_not_inet = 0;
out:
	return ret;