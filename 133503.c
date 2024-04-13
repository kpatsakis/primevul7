
static int __netdev_has_upper_dev(struct net_device *upper_dev, void *data)
{
	struct net_device *dev = data;

	return upper_dev == dev;