ep_write_iter(struct kiocb *iocb, struct iov_iter *from)
{
	struct file *file = iocb->ki_filp;
	struct ep_data *epdata = file->private_data;
	size_t len = iov_iter_count(from);
	bool configured;
	ssize_t value;
	char *buf;

	if ((value = get_ready_ep(file->f_flags, epdata, true)) < 0)
		return value;

	configured = epdata->state == STATE_EP_ENABLED;

	/* halt any endpoint by doing a "wrong direction" i/o call */
	if (configured && !usb_endpoint_dir_in(&epdata->desc)) {
		if (usb_endpoint_xfer_isoc(&epdata->desc) ||
		    !is_sync_kiocb(iocb)) {
			mutex_unlock(&epdata->lock);
			return -EINVAL;
		}
		DBG (epdata->dev, "%s halt\n", epdata->name);
		spin_lock_irq(&epdata->dev->lock);
		if (likely(epdata->ep != NULL))
			usb_ep_set_halt(epdata->ep);
		spin_unlock_irq(&epdata->dev->lock);
		mutex_unlock(&epdata->lock);
		return -EBADMSG;
	}

	buf = kmalloc(len, GFP_KERNEL);
	if (unlikely(!buf)) {
		mutex_unlock(&epdata->lock);
		return -ENOMEM;
	}

	if (unlikely(!copy_from_iter_full(buf, len, from))) {
		value = -EFAULT;
		goto out;
	}

	if (unlikely(!configured)) {
		value = ep_config(epdata, buf, len);
	} else if (is_sync_kiocb(iocb)) {
		value = ep_io(epdata, buf, len);
	} else {
		struct kiocb_priv *priv = kzalloc(sizeof *priv, GFP_KERNEL);
		value = -ENOMEM;
		if (priv) {
			value = ep_aio(iocb, priv, epdata, buf, len);
			if (value == -EIOCBQUEUED)
				buf = NULL;
		}
	}
out:
	kfree(buf);
	mutex_unlock(&epdata->lock);
	return value;
}