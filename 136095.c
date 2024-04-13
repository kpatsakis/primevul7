gadgetfs_suspend (struct usb_gadget *gadget)
{
	struct dev_data		*dev = get_gadget_data (gadget);
	unsigned long		flags;

	INFO (dev, "suspended from state %d\n", dev->state);
	spin_lock_irqsave(&dev->lock, flags);
	switch (dev->state) {
	case STATE_DEV_SETUP:		// VERY odd... host died??
	case STATE_DEV_CONNECTED:
	case STATE_DEV_UNCONNECTED:
		next_event (dev, GADGETFS_SUSPEND);
		ep0_readable (dev);
		fallthrough;
	default:
		break;
	}
	spin_unlock_irqrestore(&dev->lock, flags);
}