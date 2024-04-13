static int tweak_set_configuration_cmd(struct urb *urb)
{
	struct stub_priv *priv = (struct stub_priv *) urb->context;
	struct stub_device *sdev = priv->sdev;
	struct usb_ctrlrequest *req;
	__u16 config;
	int err;

	req = (struct usb_ctrlrequest *) urb->setup_packet;
	config = le16_to_cpu(req->wValue);

	err = usb_set_configuration(sdev->udev, config);
	if (err && err != -ENODEV)
		dev_err(&sdev->udev->dev, "can't set config #%d, error %d\n",
			config, err);
	return 0;
}