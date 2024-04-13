int xdp_do_generic_redirect(struct net_device *dev, struct sk_buff *skb,
			    struct bpf_prog *xdp_prog)
{
	struct redirect_info *ri = this_cpu_ptr(&redirect_info);
	u32 index = ri->ifindex;
	struct net_device *fwd;
	int err = 0;

	if (ri->map)
		return xdp_do_generic_redirect_map(dev, skb, xdp_prog);

	ri->ifindex = 0;
	fwd = dev_get_by_index_rcu(dev_net(dev), index);
	if (unlikely(!fwd)) {
		err = -EINVAL;
		goto err;
	}

	if (unlikely((err = __xdp_generic_ok_fwd_dev(skb, fwd))))
		goto err;

	skb->dev = fwd;
	_trace_xdp_redirect(dev, xdp_prog, index);
	return 0;
err:
	_trace_xdp_redirect_err(dev, xdp_prog, index, err);
	return err;
}