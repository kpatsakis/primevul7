static int sr9700_get_eeprom(struct net_device *netdev,
			     struct ethtool_eeprom *eeprom, u8 *data)
{
	struct usbnet *dev = netdev_priv(netdev);
	__le16 *buf = (__le16 *)data;
	int ret = 0;
	int i;

	/* access is 16bit */
	if ((eeprom->offset & 0x01) || (eeprom->len & 0x01))
		return -EINVAL;

	for (i = 0; i < eeprom->len / 2; i++) {
		ret = sr_read_eeprom_word(dev, eeprom->offset / 2 + i, buf + i);
		if (ret < 0)
			break;
	}

	return ret;
}