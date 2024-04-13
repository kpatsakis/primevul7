 */
int netdev_master_upper_dev_link(struct net_device *dev,
				 struct net_device *upper_dev,
				 void *upper_priv, void *upper_info,
				 struct netlink_ext_ack *extack)
{
	return __netdev_upper_dev_link(dev, upper_dev, true,
				       upper_priv, upper_info, extack);