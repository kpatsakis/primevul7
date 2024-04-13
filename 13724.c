static int sr_read_eeprom_word(struct usbnet *dev, u8 offset, void *value)
{
	return sr_share_read_word(dev, 0, offset, value);
}