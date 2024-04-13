int skb_do_redirect(struct sk_buff *skb)
{
	struct redirect_info *ri = this_cpu_ptr(&redirect_info);
	struct net_device *dev;

	dev = dev_get_by_index_rcu(dev_net(skb->dev), ri->ifindex);
	ri->ifindex = 0;
	if (unlikely(!dev)) {
		kfree_skb(skb);
		return -EINVAL;
	}

	return __bpf_redirect(skb, dev, ri->flags);
}