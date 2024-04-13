static int sr_mdio_read(struct net_device *netdev, int phy_id, int loc)
{
	struct usbnet *dev = netdev_priv(netdev);
	__le16 res;
	int rc = 0;

	if (phy_id) {
		netdev_dbg(netdev, "Only internal phy supported\n");
		return 0;
	}

	/* Access NSR_LINKST bit for link status instead of MII_BMSR */
	if (loc == MII_BMSR) {
		u8 value;

		sr_read_reg(dev, SR_NSR, &value);
		if (value & NSR_LINKST)
			rc = 1;
	}
	sr_share_read_word(dev, 1, loc, &res);
	if (rc == 1)
		res = le16_to_cpu(res) | BMSR_LSTATUS;
	else
		res = le16_to_cpu(res) & ~BMSR_LSTATUS;

	netdev_dbg(netdev, "sr_mdio_read() phy_id=0x%02x, loc=0x%02x, returns=0x%04x\n",
		   phy_id, loc, res);

	return res;
}