static void sr9700_set_multicast(struct net_device *netdev)
{
	struct usbnet *dev = netdev_priv(netdev);
	/* We use the 20 byte dev->data for our 8 byte filter buffer
	 * to avoid allocating memory that is tricky to free later
	 */
	u8 *hashes = (u8 *)&dev->data;
	/* rx_ctl setting : enable, disable_long, disable_crc */
	u8 rx_ctl = RCR_RXEN | RCR_DIS_CRC | RCR_DIS_LONG;

	memset(hashes, 0x00, SR_MCAST_SIZE);
	/* broadcast address */
	hashes[SR_MCAST_SIZE - 1] |= SR_MCAST_ADDR_FLAG;
	if (netdev->flags & IFF_PROMISC) {
		rx_ctl |= RCR_PRMSC;
	} else if (netdev->flags & IFF_ALLMULTI ||
		   netdev_mc_count(netdev) > SR_MCAST_MAX) {
		rx_ctl |= RCR_RUNT;
	} else if (!netdev_mc_empty(netdev)) {
		struct netdev_hw_addr *ha;

		netdev_for_each_mc_addr(ha, netdev) {
			u32 crc = ether_crc(ETH_ALEN, ha->addr) >> 26;
			hashes[crc >> 3] |= 1 << (crc & 0x7);
		}
	}

	sr_write_async(dev, SR_MAR, SR_MCAST_SIZE, hashes);
	sr_write_reg_async(dev, SR_RCR, rx_ctl);
}