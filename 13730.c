static int sr_write_reg(struct usbnet *dev, u8 reg, u8 value)
{
	return usbnet_write_cmd(dev, SR_WR_REGS, SR_REQ_WR_REG,
				value, reg, NULL, 0);
}