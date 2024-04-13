ep0_read (struct file *fd, char __user *buf, size_t len, loff_t *ptr)
{
	struct dev_data			*dev = fd->private_data;
	ssize_t				retval;
	enum ep0_state			state;

	spin_lock_irq (&dev->lock);
	if (dev->state <= STATE_DEV_OPENED) {
		retval = -EINVAL;
		goto done;
	}

	/* report fd mode change before acting on it */
	if (dev->setup_abort) {
		dev->setup_abort = 0;
		retval = -EIDRM;
		goto done;
	}

	/* control DATA stage */
	if ((state = dev->state) == STATE_DEV_SETUP) {

		if (dev->setup_in) {		/* stall IN */
			VDEBUG(dev, "ep0in stall\n");
			(void) usb_ep_set_halt (dev->gadget->ep0);
			retval = -EL2HLT;
			dev->state = STATE_DEV_CONNECTED;

		} else if (len == 0) {		/* ack SET_CONFIGURATION etc */
			struct usb_ep		*ep = dev->gadget->ep0;
			struct usb_request	*req = dev->req;

			if ((retval = setup_req (ep, req, 0)) == 0) {
				++dev->udc_usage;
				spin_unlock_irq (&dev->lock);
				retval = usb_ep_queue (ep, req, GFP_KERNEL);
				spin_lock_irq (&dev->lock);
				--dev->udc_usage;
			}
			dev->state = STATE_DEV_CONNECTED;

			/* assume that was SET_CONFIGURATION */
			if (dev->current_config) {
				unsigned power;

				if (gadget_is_dualspeed(dev->gadget)
						&& (dev->gadget->speed
							== USB_SPEED_HIGH))
					power = dev->hs_config->bMaxPower;
				else
					power = dev->config->bMaxPower;
				usb_gadget_vbus_draw(dev->gadget, 2 * power);
			}

		} else {			/* collect OUT data */
			if ((fd->f_flags & O_NONBLOCK) != 0
					&& !dev->setup_out_ready) {
				retval = -EAGAIN;
				goto done;
			}
			spin_unlock_irq (&dev->lock);
			retval = wait_event_interruptible (dev->wait,
					dev->setup_out_ready != 0);

			/* FIXME state could change from under us */
			spin_lock_irq (&dev->lock);
			if (retval)
				goto done;

			if (dev->state != STATE_DEV_SETUP) {
				retval = -ECANCELED;
				goto done;
			}
			dev->state = STATE_DEV_CONNECTED;

			if (dev->setup_out_error)
				retval = -EIO;
			else {
				len = min (len, (size_t)dev->req->actual);
				++dev->udc_usage;
				spin_unlock_irq(&dev->lock);
				if (copy_to_user (buf, dev->req->buf, len))
					retval = -EFAULT;
				else
					retval = len;
				spin_lock_irq(&dev->lock);
				--dev->udc_usage;
				clean_req (dev->gadget->ep0, dev->req);
				/* NOTE userspace can't yet choose to stall */
			}
		}
		goto done;
	}

	/* else normal: return event data */
	if (len < sizeof dev->event [0]) {
		retval = -EINVAL;
		goto done;
	}
	len -= len % sizeof (struct usb_gadgetfs_event);
	dev->usermode_setup = 1;

scan:
	/* return queued events right away */
	if (dev->ev_next != 0) {
		unsigned		i, n;

		n = len / sizeof (struct usb_gadgetfs_event);
		if (dev->ev_next < n)
			n = dev->ev_next;

		/* ep0 i/o has special semantics during STATE_DEV_SETUP */
		for (i = 0; i < n; i++) {
			if (dev->event [i].type == GADGETFS_SETUP) {
				dev->state = STATE_DEV_SETUP;
				n = i + 1;
				break;
			}
		}
		spin_unlock_irq (&dev->lock);
		len = n * sizeof (struct usb_gadgetfs_event);
		if (copy_to_user (buf, &dev->event, len))
			retval = -EFAULT;
		else
			retval = len;
		if (len > 0) {
			/* NOTE this doesn't guard against broken drivers;
			 * concurrent ep0 readers may lose events.
			 */
			spin_lock_irq (&dev->lock);
			if (dev->ev_next > n) {
				memmove(&dev->event[0], &dev->event[n],
					sizeof (struct usb_gadgetfs_event)
						* (dev->ev_next - n));
			}
			dev->ev_next -= n;
			spin_unlock_irq (&dev->lock);
		}
		return retval;
	}
	if (fd->f_flags & O_NONBLOCK) {
		retval = -EAGAIN;
		goto done;
	}

	switch (state) {
	default:
		DBG (dev, "fail %s, state %d\n", __func__, state);
		retval = -ESRCH;
		break;
	case STATE_DEV_UNCONNECTED:
	case STATE_DEV_CONNECTED:
		spin_unlock_irq (&dev->lock);
		DBG (dev, "%s wait\n", __func__);

		/* wait for events */
		retval = wait_event_interruptible (dev->wait,
				dev->ev_next != 0);
		if (retval < 0)
			return retval;
		spin_lock_irq (&dev->lock);
		goto scan;
	}

done:
	spin_unlock_irq (&dev->lock);
	return retval;
}