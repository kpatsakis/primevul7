
bool netdev_has_upper_dev_all_rcu(struct net_device *dev,
				  struct net_device *upper_dev)
{
	return !!netdev_walk_all_upper_dev_rcu(dev, __netdev_has_upper_dev,
					       upper_dev);