static int wait_phy_eeprom_ready(struct usbnet *dev, int phy)
{
	int i;

	for (i = 0; i < SR_SHARE_TIMEOUT; i++) {
		u8 tmp = 0;
		int ret;

		udelay(1);
		ret = sr_read_reg(dev, SR_EPCR, &tmp);
		if (ret < 0)
			return ret;

		/* ready */
		if (!(tmp & EPCR_ERRE))
			return 0;
	}

	netdev_err(dev->net, "%s write timed out!\n", phy ? "phy" : "eeprom");

	return -EIO;
}