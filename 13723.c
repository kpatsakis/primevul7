static int sr_share_write_word(struct usbnet *dev, int phy, u8 reg,
			       __le16 value)
{
	int ret;

	mutex_lock(&dev->phy_mutex);

	ret = sr_write(dev, SR_EPDR, 2, &value);
	if (ret < 0)
		goto out_unlock;

	sr_write_reg(dev, SR_EPAR, phy ? (reg | EPAR_PHY_ADR) : reg);
	sr_write_reg(dev, SR_EPCR, phy ? (EPCR_WEP | EPCR_EPOS | EPCR_ERPRW) :
		    (EPCR_WEP | EPCR_ERPRW));

	ret = wait_phy_eeprom_ready(dev, phy);
	if (ret < 0)
		goto out_unlock;

	sr_write_reg(dev, SR_EPCR, 0x0);

out_unlock:
	mutex_unlock(&dev->phy_mutex);
	return ret;
}