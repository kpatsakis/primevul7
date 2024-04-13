static int __bpf_tx_xdp(struct net_device *dev,
			struct bpf_map *map,
			struct xdp_buff *xdp,
			u32 index)
{
	int err;

	if (!dev->netdev_ops->ndo_xdp_xmit) {
		return -EOPNOTSUPP;
	}

	err = dev->netdev_ops->ndo_xdp_xmit(dev, xdp);
	if (err)
		return err;
	dev->netdev_ops->ndo_xdp_flush(dev);
	return 0;
}