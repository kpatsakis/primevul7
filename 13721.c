static u32 sr9700_get_link(struct net_device *netdev)
{
	struct usbnet *dev = netdev_priv(netdev);
	u8 value = 0;
	int rc = 0;

	/* Get the Link Status directly */
	sr_read_reg(dev, SR_NSR, &value);
	if (value & NSR_LINKST)
		rc = 1;

	return rc;
}