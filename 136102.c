ep0_write (struct file *fd, const char __user *buf, size_t len, loff_t *ptr)
{
	struct dev_data		*dev = fd->private_data;
	ssize_t			retval = -ESRCH;

	/* report fd mode change before acting on it */
	if (dev->setup_abort) {
		dev->setup_abort = 0;
		retval = -EIDRM;

	/* data and/or status stage for control request */
	} else if (dev->state == STATE_DEV_SETUP) {

		len = min_t(size_t, len, dev->setup_wLength);
		if (dev->setup_in) {
			retval = setup_req (dev->gadget->ep0, dev->req, len);
			if (retval == 0) {
				dev->state = STATE_DEV_CONNECTED;
				++dev->udc_usage;
				spin_unlock_irq (&dev->lock);
				if (copy_from_user (dev->req->buf, buf, len))
					retval = -EFAULT;
				else {
					if (len < dev->setup_wLength)
						dev->req->zero = 1;
					retval = usb_ep_queue (
						dev->gadget->ep0, dev->req,
						GFP_KERNEL);
				}
				spin_lock_irq(&dev->lock);
				--dev->udc_usage;
				if (retval < 0) {
					clean_req (dev->gadget->ep0, dev->req);
				} else
					retval = len;

				return retval;
			}

		/* can stall some OUT transfers */
		} else if (dev->setup_can_stall) {
			VDEBUG(dev, "ep0out stall\n");
			(void) usb_ep_set_halt (dev->gadget->ep0);
			retval = -EL2HLT;
			dev->state = STATE_DEV_CONNECTED;
		} else {
			DBG(dev, "bogus ep0out stall!\n");
		}
	} else
		DBG (dev, "fail %s, state %d\n", __func__, dev->state);

	return retval;
}