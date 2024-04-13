static int xdp_do_generic_redirect_map(struct net_device *dev,
				       struct sk_buff *skb,
				       struct bpf_prog *xdp_prog)
{
	struct redirect_info *ri = this_cpu_ptr(&redirect_info);
	unsigned long map_owner = ri->map_owner;
	struct bpf_map *map = ri->map;
	struct net_device *fwd = NULL;
	u32 index = ri->ifindex;
	int err = 0;

	ri->ifindex = 0;
	ri->map = NULL;
	ri->map_owner = 0;

	if (unlikely(xdp_map_invalid(xdp_prog, map_owner))) {
		err = -EFAULT;
		map = NULL;
		goto err;
	}
	fwd = __xdp_map_lookup_elem(map, index);
	if (unlikely(!fwd)) {
		err = -EINVAL;
		goto err;
	}

	if (map->map_type == BPF_MAP_TYPE_DEVMAP) {
		if (unlikely((err = __xdp_generic_ok_fwd_dev(skb, fwd))))
			goto err;
		skb->dev = fwd;
	} else {
		/* TODO: Handle BPF_MAP_TYPE_CPUMAP */
		err = -EBADRQC;
		goto err;
	}

	_trace_xdp_redirect_map(dev, xdp_prog, fwd, map, index);
	return 0;
err:
	_trace_xdp_redirect_map_err(dev, xdp_prog, fwd, map, index, err);
	return err;
}