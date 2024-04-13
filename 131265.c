static int dvb_frontend_get_event(struct dvb_frontend *fe,
			    struct dvb_frontend_event *event, int flags)
{
	struct dvb_frontend_private *fepriv = fe->frontend_priv;
	struct dvb_fe_events *events = &fepriv->events;

	dev_dbg(fe->dvb->device, "%s:\n", __func__);

	if (events->overflow) {
		events->overflow = 0;
		return -EOVERFLOW;
	}

	if (events->eventw == events->eventr) {
		int ret;

		if (flags & O_NONBLOCK)
			return -EWOULDBLOCK;

		up(&fepriv->sem);

		ret = wait_event_interruptible (events->wait_queue,
						events->eventw != events->eventr);

		if (down_interruptible (&fepriv->sem))
			return -ERESTARTSYS;

		if (ret < 0)
			return ret;
	}

	mutex_lock(&events->mtx);
	*event = events->events[events->eventr];
	events->eventr = (events->eventr + 1) % MAX_EVENT;
	mutex_unlock(&events->mtx);

	return 0;
}