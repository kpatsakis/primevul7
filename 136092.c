get_ready_ep (unsigned f_flags, struct ep_data *epdata, bool is_write)
{
	int	val;

	if (f_flags & O_NONBLOCK) {
		if (!mutex_trylock(&epdata->lock))
			goto nonblock;
		if (epdata->state != STATE_EP_ENABLED &&
		    (!is_write || epdata->state != STATE_EP_READY)) {
			mutex_unlock(&epdata->lock);
nonblock:
			val = -EAGAIN;
		} else
			val = 0;
		return val;
	}

	val = mutex_lock_interruptible(&epdata->lock);
	if (val < 0)
		return val;

	switch (epdata->state) {
	case STATE_EP_ENABLED:
		return 0;
	case STATE_EP_READY:			/* not configured yet */
		if (is_write)
			return 0;
		fallthrough;
	case STATE_EP_UNBOUND:			/* clean disconnect */
		break;
	// case STATE_EP_DISABLED:		/* "can't happen" */
	default:				/* error! */
		pr_debug ("%s: ep %p not available, state %d\n",
				shortname, epdata, epdata->state);
	}
	mutex_unlock(&epdata->lock);
	return -ENODEV;
}