 */
int dev_change_proto_down(struct net_device *dev, bool proto_down)
{
	const struct net_device_ops *ops = dev->netdev_ops;

	if (!ops->ndo_change_proto_down)
		return -EOPNOTSUPP;
	if (!netif_device_present(dev))
		return -ENODEV;
	return ops->ndo_change_proto_down(dev, proto_down);