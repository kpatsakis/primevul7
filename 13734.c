static int sr9700_set_mac_address(struct net_device *netdev, void *p)
{
	struct usbnet *dev = netdev_priv(netdev);
	struct sockaddr *addr = p;

	if (!is_valid_ether_addr(addr->sa_data)) {
		netdev_err(netdev, "not setting invalid mac address %pM\n",
			   addr->sa_data);
		return -EINVAL;
	}

	eth_hw_addr_set(netdev, addr->sa_data);
	sr_write_async(dev, SR_PAR, 6, netdev->dev_addr);

	return 0;
}