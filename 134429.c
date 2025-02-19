static void adu_disconnect(struct usb_interface *interface)
{
	struct adu_device *dev;

	dev = usb_get_intfdata(interface);

	usb_deregister_dev(interface, &adu_class);

	mutex_lock(&adutux_mutex);
	usb_set_intfdata(interface, NULL);

	mutex_lock(&dev->mtx);	/* not interruptible */
	dev->udev = NULL;	/* poison */
	mutex_unlock(&dev->mtx);

	/* if the device is not opened, then we clean up right now */
	if (!dev->open_count)
		adu_delete(dev);

	mutex_unlock(&adutux_mutex);
}