ep_read_iter(struct kiocb *iocb, struct iov_iter *to)
{
	struct file *file = iocb->ki_filp;
	struct ep_data *epdata = file->private_data;
	size_t len = iov_iter_count(to);
	ssize_t value;
	char *buf;

	if ((value = get_ready_ep(file->f_flags, epdata, false)) < 0)
		return value;

	/* halt any endpoint by doing a "wrong direction" i/o call */
	if (usb_endpoint_dir_in(&epdata->desc)) {
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
	if (is_sync_kiocb(iocb)) {
		value = ep_io(epdata, buf, len);
		if (value >= 0 && (copy_to_iter(buf, value, to) != value))
			value = -EFAULT;
	} else {
		struct kiocb_priv *priv = kzalloc(sizeof *priv, GFP_KERNEL);
		value = -ENOMEM;
		if (!priv)
			goto fail;
		priv->to_free = dup_iter(&priv->to, to, GFP_KERNEL);
		if (!priv->to_free) {
			kfree(priv);
			goto fail;
		}
		value = ep_aio(iocb, priv, epdata, buf, len);
		if (value == -EIOCBQUEUED)
			buf = NULL;
	}
fail:
	kfree(buf);
	mutex_unlock(&epdata->lock);
	return value;
}