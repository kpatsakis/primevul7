static void sr_write_reg_async(struct usbnet *dev, u8 reg, u8 value)
{
	usbnet_write_cmd_async(dev, SR_WR_REGS, SR_REQ_WR_REG,
			       value, reg, NULL, 0);
}