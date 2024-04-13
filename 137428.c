static int tweak_set_interface_cmd(struct urb *urb)
{
	struct usb_ctrlrequest *req;
	__u16 alternate;
	__u16 interface;
	int ret;

	req = (struct usb_ctrlrequest *) urb->setup_packet;
	alternate = le16_to_cpu(req->wValue);
	interface = le16_to_cpu(req->wIndex);

	usbip_dbg_stub_rx("set_interface: inf %u alt %u\n",
			  interface, alternate);

	ret = usb_set_interface(urb->dev, interface, alternate);
	if (ret < 0)
		dev_err(&urb->dev->dev,
			"usb_set_interface error: inf %u alt %u ret %d\n",
			interface, alternate, ret);
	else
		dev_info(&urb->dev->dev,
			"usb_set_interface done: inf %u alt %u\n",
			interface, alternate);

	return ret;
}