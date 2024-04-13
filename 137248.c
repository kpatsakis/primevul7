BPF_CALL_3(bpf_clone_redirect, struct sk_buff *, skb, u32, ifindex, u64, flags)
{
	struct net_device *dev;
	struct sk_buff *clone;
	int ret;

	if (unlikely(flags & ~(BPF_F_INGRESS)))
		return -EINVAL;

	dev = dev_get_by_index_rcu(dev_net(skb->dev), ifindex);
	if (unlikely(!dev))
		return -EINVAL;

	clone = skb_clone(skb, GFP_ATOMIC);
	if (unlikely(!clone))
		return -ENOMEM;

	/* For direct write, we need to keep the invariant that the skbs
	 * we're dealing with need to be uncloned. Should uncloning fail
	 * here, we need to free the just generated clone to unclone once
	 * again.
	 */
	ret = bpf_try_make_head_writable(skb);
	if (unlikely(ret)) {
		kfree_skb(clone);
		return -ENOMEM;
	}

	return __bpf_redirect(clone, dev, flags);
}