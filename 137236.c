static int __bpf_tx_xdp_map(struct net_device *dev_rx, void *fwd,
			    struct bpf_map *map,
			    struct xdp_buff *xdp,
			    u32 index)
{
	int err;

	if (map->map_type == BPF_MAP_TYPE_DEVMAP) {
		struct net_device *dev = fwd;

		if (!dev->netdev_ops->ndo_xdp_xmit)
			return -EOPNOTSUPP;

		err = dev->netdev_ops->ndo_xdp_xmit(dev, xdp);
		if (err)
			return err;
		__dev_map_insert_ctx(map, index);

	} else if (map->map_type == BPF_MAP_TYPE_CPUMAP) {
		struct bpf_cpu_map_entry *rcpu = fwd;

		err = cpu_map_enqueue(rcpu, xdp, dev_rx);
		if (err)
			return err;
		__cpu_map_insert_ctx(map, index);
	}
	return 0;
}