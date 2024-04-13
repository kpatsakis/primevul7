static int get_pipe(struct stub_device *sdev, struct usbip_header *pdu)
{
	struct usb_device *udev = sdev->udev;
	struct usb_host_endpoint *ep;
	struct usb_endpoint_descriptor *epd = NULL;
	int epnum = pdu->base.ep;
	int dir = pdu->base.direction;

	if (epnum < 0 || epnum > 15)
		goto err_ret;

	if (dir == USBIP_DIR_IN)
		ep = udev->ep_in[epnum & 0x7f];
	else
		ep = udev->ep_out[epnum & 0x7f];
	if (!ep)
		goto err_ret;

	epd = &ep->desc;

	/* validate transfer_buffer_length */
	if (pdu->u.cmd_submit.transfer_buffer_length > INT_MAX) {
		dev_err(&sdev->udev->dev,
			"CMD_SUBMIT: -EMSGSIZE transfer_buffer_length %d\n",
			pdu->u.cmd_submit.transfer_buffer_length);
		return -1;
	}

	if (usb_endpoint_xfer_control(epd)) {
		if (dir == USBIP_DIR_OUT)
			return usb_sndctrlpipe(udev, epnum);
		else
			return usb_rcvctrlpipe(udev, epnum);
	}

	if (usb_endpoint_xfer_bulk(epd)) {
		if (dir == USBIP_DIR_OUT)
			return usb_sndbulkpipe(udev, epnum);
		else
			return usb_rcvbulkpipe(udev, epnum);
	}

	if (usb_endpoint_xfer_int(epd)) {
		if (dir == USBIP_DIR_OUT)
			return usb_sndintpipe(udev, epnum);
		else
			return usb_rcvintpipe(udev, epnum);
	}

	if (usb_endpoint_xfer_isoc(epd)) {
		/* validate packet size and number of packets */
		unsigned int maxp, packets, bytes;

		maxp = usb_endpoint_maxp(epd);
		maxp *= usb_endpoint_maxp_mult(epd);
		bytes = pdu->u.cmd_submit.transfer_buffer_length;
		packets = DIV_ROUND_UP(bytes, maxp);

		if (pdu->u.cmd_submit.number_of_packets < 0 ||
		    pdu->u.cmd_submit.number_of_packets > packets) {
			dev_err(&sdev->udev->dev,
				"CMD_SUBMIT: isoc invalid num packets %d\n",
				pdu->u.cmd_submit.number_of_packets);
			return -1;
		}
		if (dir == USBIP_DIR_OUT)
			return usb_sndisocpipe(udev, epnum);
		else
			return usb_rcvisocpipe(udev, epnum);
	}

err_ret:
	/* NOT REACHED */
	dev_err(&sdev->udev->dev, "CMD_SUBMIT: invalid epnum %d\n", epnum);
	return -1;
}