static void sr_mdio_write(struct net_device *netdev, int phy_id, int loc,
			  int val)
{
	struct usbnet *dev = netdev_priv(netdev);
	__le16 res = cpu_to_le16(val);

	if (phy_id) {
		netdev_dbg(netdev, "Only internal phy supported\n");
		return;
	}

	netdev_dbg(netdev, "sr_mdio_write() phy_id=0x%02x, loc=0x%02x, val=0x%04x\n",
		   phy_id, loc, val);

	sr_share_write_word(dev, 1, loc, res);
}