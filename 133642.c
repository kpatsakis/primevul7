
static int __netdev_upper_dev_link(struct net_device *dev,
				   struct net_device *upper_dev, bool master,
				   void *upper_priv, void *upper_info,
				   struct netlink_ext_ack *extack)
{
	struct netdev_notifier_changeupper_info changeupper_info = {
		.info = {
			.dev = dev,
			.extack = extack,
		},
		.upper_dev = upper_dev,
		.master = master,
		.linking = true,
		.upper_info = upper_info,
	};
	struct net_device *master_dev;
	int ret = 0;

	ASSERT_RTNL();

	if (dev == upper_dev)
		return -EBUSY;

	/* To prevent loops, check if dev is not upper device to upper_dev. */
	if (netdev_has_upper_dev(upper_dev, dev))
		return -EBUSY;

	if (!master) {
		if (netdev_has_upper_dev(dev, upper_dev))
			return -EEXIST;
	} else {
		master_dev = netdev_master_upper_dev_get(dev);
		if (master_dev)
			return master_dev == upper_dev ? -EEXIST : -EBUSY;
	}

	ret = call_netdevice_notifiers_info(NETDEV_PRECHANGEUPPER,
					    &changeupper_info.info);
	ret = notifier_to_errno(ret);
	if (ret)
		return ret;

	ret = __netdev_adjacent_dev_link_neighbour(dev, upper_dev, upper_priv,
						   master);
	if (ret)
		return ret;

	ret = call_netdevice_notifiers_info(NETDEV_CHANGEUPPER,
					    &changeupper_info.info);
	ret = notifier_to_errno(ret);
	if (ret)
		goto rollback;

	return 0;

rollback:
	__netdev_adjacent_dev_unlink_neighbour(dev, upper_dev);

	return ret;