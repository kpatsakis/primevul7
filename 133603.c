 */
int dev_get_phys_port_name(struct net_device *dev,
			   char *name, size_t len)
{
	const struct net_device_ops *ops = dev->netdev_ops;
	int err;

	if (ops->ndo_get_phys_port_name) {
		err = ops->ndo_get_phys_port_name(dev, name, len);
		if (err != -EOPNOTSUPP)
			return err;
	}
	return devlink_compat_phys_port_name_get(dev, name, len);