static void sr_write_async(struct usbnet *dev, u8 reg, u16 length,
			   const void *data)
{
	usbnet_write_cmd_async(dev, SR_WR_REGS, SR_REQ_WR_REG,
			       0, reg, data, length);
}