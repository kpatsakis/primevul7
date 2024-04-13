gadgetfs_setup (struct usb_gadget *gadget, const struct usb_ctrlrequest *ctrl)
{
	struct dev_data			*dev = get_gadget_data (gadget);
	struct usb_request		*req = dev->req;
	int				value = -EOPNOTSUPP;
	struct usb_gadgetfs_event	*event;
	u16				w_value = le16_to_cpu(ctrl->wValue);
	u16				w_length = le16_to_cpu(ctrl->wLength);

	if (w_length > RBUF_SIZE) {
		if (ctrl->bRequestType & USB_DIR_IN) {
			/* Cast away the const, we are going to overwrite on purpose. */
			__le16 *temp = (__le16 *)&ctrl->wLength;

			*temp = cpu_to_le16(RBUF_SIZE);
			w_length = RBUF_SIZE;
		} else {
			return value;
		}
	}

	spin_lock (&dev->lock);
	dev->setup_abort = 0;
	if (dev->state == STATE_DEV_UNCONNECTED) {
		if (gadget_is_dualspeed(gadget)
				&& gadget->speed == USB_SPEED_HIGH
				&& dev->hs_config == NULL) {
			spin_unlock(&dev->lock);
			ERROR (dev, "no high speed config??\n");
			return -EINVAL;
		}

		dev->state = STATE_DEV_CONNECTED;

		INFO (dev, "connected\n");
		event = next_event (dev, GADGETFS_CONNECT);
		event->u.speed = gadget->speed;
		ep0_readable (dev);

	/* host may have given up waiting for response.  we can miss control
	 * requests handled lower down (device/endpoint status and features);
	 * then ep0_{read,write} will report the wrong status. controller
	 * driver will have aborted pending i/o.
	 */
	} else if (dev->state == STATE_DEV_SETUP)
		dev->setup_abort = 1;

	req->buf = dev->rbuf;
	req->context = NULL;
	switch (ctrl->bRequest) {

	case USB_REQ_GET_DESCRIPTOR:
		if (ctrl->bRequestType != USB_DIR_IN)
			goto unrecognized;
		switch (w_value >> 8) {

		case USB_DT_DEVICE:
			value = min (w_length, (u16) sizeof *dev->dev);
			dev->dev->bMaxPacketSize0 = dev->gadget->ep0->maxpacket;
			req->buf = dev->dev;
			break;
		case USB_DT_DEVICE_QUALIFIER:
			if (!dev->hs_config)
				break;
			value = min (w_length, (u16)
				sizeof (struct usb_qualifier_descriptor));
			make_qualifier (dev);
			break;
		case USB_DT_OTHER_SPEED_CONFIG:
		case USB_DT_CONFIG:
			value = config_buf (dev,
					w_value >> 8,
					w_value & 0xff);
			if (value >= 0)
				value = min (w_length, (u16) value);
			break;
		case USB_DT_STRING:
			goto unrecognized;

		default:		// all others are errors
			break;
		}
		break;

	/* currently one config, two speeds */
	case USB_REQ_SET_CONFIGURATION:
		if (ctrl->bRequestType != 0)
			goto unrecognized;
		if (0 == (u8) w_value) {
			value = 0;
			dev->current_config = 0;
			usb_gadget_vbus_draw(gadget, 8 /* mA */ );
			// user mode expected to disable endpoints
		} else {
			u8	config, power;

			if (gadget_is_dualspeed(gadget)
					&& gadget->speed == USB_SPEED_HIGH) {
				config = dev->hs_config->bConfigurationValue;
				power = dev->hs_config->bMaxPower;
			} else {
				config = dev->config->bConfigurationValue;
				power = dev->config->bMaxPower;
			}

			if (config == (u8) w_value) {
				value = 0;
				dev->current_config = config;
				usb_gadget_vbus_draw(gadget, 2 * power);
			}
		}

		/* report SET_CONFIGURATION like any other control request,
		 * except that usermode may not stall this.  the next
		 * request mustn't be allowed start until this finishes:
		 * endpoints and threads set up, etc.
		 *
		 * NOTE:  older PXA hardware (before PXA 255: without UDCCFR)
		 * has bad/racey automagic that prevents synchronizing here.
		 * even kernel mode drivers often miss them.
		 */
		if (value == 0) {
			INFO (dev, "configuration #%d\n", dev->current_config);
			usb_gadget_set_state(gadget, USB_STATE_CONFIGURED);
			if (dev->usermode_setup) {
				dev->setup_can_stall = 0;
				goto delegate;
			}
		}
		break;

#ifndef	CONFIG_USB_PXA25X
	/* PXA automagically handles this request too */
	case USB_REQ_GET_CONFIGURATION:
		if (ctrl->bRequestType != 0x80)
			goto unrecognized;
		*(u8 *)req->buf = dev->current_config;
		value = min (w_length, (u16) 1);
		break;
#endif

	default:
unrecognized:
		VDEBUG (dev, "%s req%02x.%02x v%04x i%04x l%d\n",
			dev->usermode_setup ? "delegate" : "fail",
			ctrl->bRequestType, ctrl->bRequest,
			w_value, le16_to_cpu(ctrl->wIndex), w_length);

		/* if there's an ep0 reader, don't stall */
		if (dev->usermode_setup) {
			dev->setup_can_stall = 1;
delegate:
			dev->setup_in = (ctrl->bRequestType & USB_DIR_IN)
						? 1 : 0;
			dev->setup_wLength = w_length;
			dev->setup_out_ready = 0;
			dev->setup_out_error = 0;

			/* read DATA stage for OUT right away */
			if (unlikely (!dev->setup_in && w_length)) {
				value = setup_req (gadget->ep0, dev->req,
							w_length);
				if (value < 0)
					break;

				++dev->udc_usage;
				spin_unlock (&dev->lock);
				value = usb_ep_queue (gadget->ep0, dev->req,
							GFP_KERNEL);
				spin_lock (&dev->lock);
				--dev->udc_usage;
				if (value < 0) {
					clean_req (gadget->ep0, dev->req);
					break;
				}

				/* we can't currently stall these */
				dev->setup_can_stall = 0;
			}

			/* state changes when reader collects event */
			event = next_event (dev, GADGETFS_SETUP);
			event->u.setup = *ctrl;
			ep0_readable (dev);
			spin_unlock (&dev->lock);
			return 0;
		}
	}

	/* proceed with data transfer and status phases? */
	if (value >= 0 && dev->state != STATE_DEV_SETUP) {
		req->length = value;
		req->zero = value < w_length;

		++dev->udc_usage;
		spin_unlock (&dev->lock);
		value = usb_ep_queue (gadget->ep0, req, GFP_KERNEL);
		spin_lock(&dev->lock);
		--dev->udc_usage;
		spin_unlock(&dev->lock);
		if (value < 0) {
			DBG (dev, "ep_queue --> %d\n", value);
			req->status = 0;
		}
		return value;
	}

	/* device stalls when value < 0 */
	spin_unlock (&dev->lock);
	return value;
}