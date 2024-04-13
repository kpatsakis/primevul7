
int dev_set_allmulti(struct net_device *dev, int inc)
{
	return __dev_set_allmulti(dev, inc, true);