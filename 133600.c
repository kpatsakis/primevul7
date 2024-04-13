
int netdev_walk_all_upper_dev_rcu(struct net_device *dev,
				  int (*fn)(struct net_device *dev,
					    void *data),
				  void *data)
{
	struct net_device *udev;
	struct list_head *iter;
	int ret;

	for (iter = &dev->adj_list.upper,
	     udev = netdev_next_upper_dev_rcu(dev, &iter);
	     udev;
	     udev = netdev_next_upper_dev_rcu(dev, &iter)) {
		/* first is the upper device itself */
		ret = fn(udev, data);
		if (ret)
			return ret;

		/* then look at all of its upper devices */
		ret = netdev_walk_all_upper_dev_rcu(udev, fn, data);
		if (ret)
			return ret;
	}

	return 0;