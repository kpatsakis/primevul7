
int dev_get_nest_level(struct net_device *dev)
{
	struct net_device *lower = NULL;
	struct list_head *iter;
	int max_nest = -1;
	int nest;

	ASSERT_RTNL();

	netdev_for_each_lower_dev(dev, lower, iter) {
		nest = dev_get_nest_level(lower);
		if (max_nest < nest)
			max_nest = nest;
	}

	return max_nest + 1;