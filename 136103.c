ep0_fasync (int f, struct file *fd, int on)
{
	struct dev_data		*dev = fd->private_data;
	// caller must F_SETOWN before signal delivery happens
	VDEBUG (dev, "%s %s\n", __func__, on ? "on" : "off");
	return fasync_helper (f, fd, on, &dev->fasync);
}