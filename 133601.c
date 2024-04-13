 */
void netdev_upper_dev_unlink(struct net_device *dev,
			     struct net_device *upper_dev)
{
	struct netdev_notifier_changeupper_info changeupper_info = {
		.info = {
			.dev = dev,
		},
		.upper_dev = upper_dev,
		.linking = false,
	};

	ASSERT_RTNL();

	changeupper_info.master = netdev_master_upper_dev_get(dev) == upper_dev;

	call_netdevice_notifiers_info(NETDEV_PRECHANGEUPPER,
				      &changeupper_info.info);

	__netdev_adjacent_dev_unlink_neighbour(dev, upper_dev);

	call_netdevice_notifiers_info(NETDEV_CHANGEUPPER,
				      &changeupper_info.info);