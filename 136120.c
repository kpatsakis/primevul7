ep_config (struct ep_data *data, const char *buf, size_t len)
{
	struct usb_ep		*ep;
	u32			tag;
	int			value, length = len;

	if (data->state != STATE_EP_READY) {
		value = -EL2HLT;
		goto fail;
	}

	value = len;
	if (len < USB_DT_ENDPOINT_SIZE + 4)
		goto fail0;

	/* we might need to change message format someday */
	memcpy(&tag, buf, 4);
	if (tag != 1) {
		DBG(data->dev, "config %s, bad tag %d\n", data->name, tag);
		goto fail0;
	}
	buf += 4;
	len -= 4;

	/* NOTE:  audio endpoint extensions not accepted here;
	 * just don't include the extra bytes.
	 */

	/* full/low speed descriptor, then high speed */
	memcpy(&data->desc, buf, USB_DT_ENDPOINT_SIZE);
	if (data->desc.bLength != USB_DT_ENDPOINT_SIZE
			|| data->desc.bDescriptorType != USB_DT_ENDPOINT)
		goto fail0;
	if (len != USB_DT_ENDPOINT_SIZE) {
		if (len != 2 * USB_DT_ENDPOINT_SIZE)
			goto fail0;
		memcpy(&data->hs_desc, buf + USB_DT_ENDPOINT_SIZE,
			USB_DT_ENDPOINT_SIZE);
		if (data->hs_desc.bLength != USB_DT_ENDPOINT_SIZE
				|| data->hs_desc.bDescriptorType
					!= USB_DT_ENDPOINT) {
			DBG(data->dev, "config %s, bad hs length or type\n",
					data->name);
			goto fail0;
		}
	}

	spin_lock_irq (&data->dev->lock);
	if (data->dev->state == STATE_DEV_UNBOUND) {
		value = -ENOENT;
		goto gone;
	} else {
		ep = data->ep;
		if (ep == NULL) {
			value = -ENODEV;
			goto gone;
		}
	}
	switch (data->dev->gadget->speed) {
	case USB_SPEED_LOW:
	case USB_SPEED_FULL:
		ep->desc = &data->desc;
		break;
	case USB_SPEED_HIGH:
		/* fails if caller didn't provide that descriptor... */
		ep->desc = &data->hs_desc;
		break;
	default:
		DBG(data->dev, "unconnected, %s init abandoned\n",
				data->name);
		value = -EINVAL;
		goto gone;
	}
	value = usb_ep_enable(ep);
	if (value == 0) {
		data->state = STATE_EP_ENABLED;
		value = length;
	}
gone:
	spin_unlock_irq (&data->dev->lock);
	if (value < 0) {
fail:
		data->desc.bDescriptorType = 0;
		data->hs_desc.bDescriptorType = 0;
	}
	return value;
fail0:
	value = -EINVAL;
	goto fail;
}