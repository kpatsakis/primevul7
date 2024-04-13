 */
int dev_get_port_parent_id(struct net_device *dev,
			   struct netdev_phys_item_id *ppid,
			   bool recurse)
{
	const struct net_device_ops *ops = dev->netdev_ops;
	struct netdev_phys_item_id first = { };
	struct net_device *lower_dev;
	struct list_head *iter;
	int err;

	if (ops->ndo_get_port_parent_id) {
		err = ops->ndo_get_port_parent_id(dev, ppid);
		if (err != -EOPNOTSUPP)
			return err;
	}

	err = devlink_compat_switch_id_get(dev, ppid);
	if (!err || err != -EOPNOTSUPP)
		return err;

	if (!recurse)
		return -EOPNOTSUPP;

	netdev_for_each_lower_dev(dev, lower_dev, iter) {
		err = dev_get_port_parent_id(lower_dev, ppid, recurse);
		if (err)
			break;
		if (!first.id_len)
			first = *ppid;
		else if (memcmp(&first, ppid, sizeof(*ppid)))
			return -ENODATA;
	}

	return err;