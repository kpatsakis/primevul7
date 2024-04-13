static int sr_write(struct usbnet *dev, u8 reg, u16 length, void *data)
{
	int err;

	err = usbnet_write_cmd(dev, SR_WR_REGS, SR_REQ_WR_REG, 0, reg, data,
			       length);
	if ((err >= 0) && (err < length))
		err = -EINVAL;
	return err;
}