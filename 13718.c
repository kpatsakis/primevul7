static int sr9700_ioctl(struct net_device *netdev, struct ifreq *rq, int cmd)
{
	struct usbnet *dev = netdev_priv(netdev);

	return generic_mii_ioctl(&dev->mii, if_mii(rq), cmd, NULL);
}