static int sr_share_read_word(struct usbnet *dev, int phy, u8 reg,
			      __le16 *value)
{
	int ret;

	mutex_lock(&dev->phy_mutex);

	sr_write_reg(dev, SR_EPAR, phy ? (reg | EPAR_PHY_ADR) : reg);
	sr_write_reg(dev, SR_EPCR, phy ? (EPCR_EPOS | EPCR_ERPRR) : EPCR_ERPRR);

	ret = wait_phy_eeprom_ready(dev, phy);
	if (ret < 0)
		goto out_unlock;

	sr_write_reg(dev, SR_EPCR, 0x0);
	ret = sr_read(dev, SR_EPDR, 2, value);

	netdev_dbg(dev->net, "read shared %d 0x%02x returned 0x%04x, %d\n",
		   phy, reg, *value, ret);

out_unlock:
	mutex_unlock(&dev->phy_mutex);
	return ret;
}