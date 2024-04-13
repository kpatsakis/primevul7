static int xdp_do_redirect_map(struct net_device *dev, struct xdp_buff *xdp,
			       struct bpf_prog *xdp_prog)
{
	struct redirect_info *ri = this_cpu_ptr(&redirect_info);
	unsigned long map_owner = ri->map_owner;
	struct bpf_map *map = ri->map;
	u32 index = ri->ifindex;
	void *fwd = NULL;
	int err;

	ri->ifindex = 0;
	ri->map = NULL;
	ri->map_owner = 0;

	if (unlikely(xdp_map_invalid(xdp_prog, map_owner))) {
		err = -EFAULT;
		map = NULL;
		goto err;
	}

	fwd = __xdp_map_lookup_elem(map, index);
	if (!fwd) {
		err = -EINVAL;
		goto err;
	}
	if (ri->map_to_flush && ri->map_to_flush != map)
		xdp_do_flush_map();

	err = __bpf_tx_xdp_map(dev, fwd, map, xdp, index);
	if (unlikely(err))
		goto err;

	ri->map_to_flush = map;
	_trace_xdp_redirect_map(dev, xdp_prog, fwd, map, index);
	return 0;
err:
	_trace_xdp_redirect_map_err(dev, xdp_prog, fwd, map, index, err);
	return err;
}