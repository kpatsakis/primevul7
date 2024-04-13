dev_release (struct inode *inode, struct file *fd)
{
	struct dev_data		*dev = fd->private_data;

	/* closing ep0 === shutdown all */

	if (dev->gadget_registered) {
		usb_gadget_unregister_driver (&gadgetfs_driver);
		dev->gadget_registered = false;
	}

	/* at this point "good" hardware has disconnected the
	 * device from USB; the host won't see it any more.
	 * alternatively, all host requests will time out.
	 */

	kfree (dev->buf);
	dev->buf = NULL;

	/* other endpoints were all decoupled from this device */
	spin_lock_irq(&dev->lock);
	dev->state = STATE_DEV_DISABLED;
	spin_unlock_irq(&dev->lock);

	put_dev (dev);
	return 0;
}