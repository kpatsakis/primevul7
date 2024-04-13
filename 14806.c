static int ax_open_dev(struct net_device *dev)
{
	struct mkiss *ax = netdev_priv(dev);

	if (ax->tty == NULL)
		return -ENODEV;

	return 0;
}