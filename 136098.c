ep0_poll (struct file *fd, poll_table *wait)
{
	struct dev_data         *dev = fd->private_data;
	__poll_t                mask = 0;

	if (dev->state <= STATE_DEV_OPENED)
		return DEFAULT_POLLMASK;

	poll_wait(fd, &dev->wait, wait);

	spin_lock_irq(&dev->lock);

	/* report fd mode change before acting on it */
	if (dev->setup_abort) {
		dev->setup_abort = 0;
		mask = EPOLLHUP;
		goto out;
	}

	if (dev->state == STATE_DEV_SETUP) {
		if (dev->setup_in || dev->setup_can_stall)
			mask = EPOLLOUT;
	} else {
		if (dev->ev_next != 0)
			mask = EPOLLIN;
	}
out:
	spin_unlock_irq(&dev->lock);
	return mask;
}