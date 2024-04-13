int ipmi_set_gets_events(struct ipmi_user *user, bool val)
{
	unsigned long        flags;
	struct ipmi_smi      *intf = user->intf;
	struct ipmi_recv_msg *msg, *msg2;
	struct list_head     msgs;
	int index;

	user = acquire_ipmi_user(user, &index);
	if (!user)
		return -ENODEV;

	INIT_LIST_HEAD(&msgs);

	spin_lock_irqsave(&intf->events_lock, flags);
	if (user->gets_events == val)
		goto out;

	user->gets_events = val;

	if (val) {
		if (atomic_inc_return(&intf->event_waiters) == 1)
			need_waiter(intf);
	} else {
		atomic_dec(&intf->event_waiters);
	}

	if (intf->delivering_events)
		/*
		 * Another thread is delivering events for this, so
		 * let it handle any new events.
		 */
		goto out;

	/* Deliver any queued events. */
	while (user->gets_events && !list_empty(&intf->waiting_events)) {
		list_for_each_entry_safe(msg, msg2, &intf->waiting_events, link)
			list_move_tail(&msg->link, &msgs);
		intf->waiting_events_count = 0;
		if (intf->event_msg_printed) {
			dev_warn(intf->si_dev, "Event queue no longer full\n");
			intf->event_msg_printed = 0;
		}

		intf->delivering_events = 1;
		spin_unlock_irqrestore(&intf->events_lock, flags);

		list_for_each_entry_safe(msg, msg2, &msgs, link) {
			msg->user = user;
			kref_get(&user->refcount);
			deliver_local_response(intf, msg);
		}

		spin_lock_irqsave(&intf->events_lock, flags);
		intf->delivering_events = 0;
	}

 out:
	spin_unlock_irqrestore(&intf->events_lock, flags);
	release_ipmi_user(user, index);

	return 0;
}