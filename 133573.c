 */
int dev_pre_changeaddr_notify(struct net_device *dev, const char *addr,
			      struct netlink_ext_ack *extack)
{
	struct netdev_notifier_pre_changeaddr_info info = {
		.info.dev = dev,
		.info.extack = extack,
		.dev_addr = addr,
	};
	int rc;

	rc = call_netdevice_notifiers_info(NETDEV_PRE_CHANGEADDR, &info.info);
	return notifier_to_errno(rc);