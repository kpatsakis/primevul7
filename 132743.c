static int panic_event(struct notifier_block *this,
		       unsigned long         event,
		       void                  *ptr)
{
	struct ipmi_smi *intf;
	struct ipmi_user *user;

	if (has_panicked)
		return NOTIFY_DONE;
	has_panicked = 1;

	/* For every registered interface, set it to run to completion. */
	list_for_each_entry_rcu(intf, &ipmi_interfaces, link) {
		if (!intf->handlers || intf->intf_num == -1)
			/* Interface is not ready. */
			continue;

		if (!intf->handlers->poll)
			continue;

		/*
		 * If we were interrupted while locking xmit_msgs_lock or
		 * waiting_rcv_msgs_lock, the corresponding list may be
		 * corrupted.  In this case, drop items on the list for
		 * the safety.
		 */
		if (!spin_trylock(&intf->xmit_msgs_lock)) {
			INIT_LIST_HEAD(&intf->xmit_msgs);
			INIT_LIST_HEAD(&intf->hp_xmit_msgs);
		} else
			spin_unlock(&intf->xmit_msgs_lock);

		if (!spin_trylock(&intf->waiting_rcv_msgs_lock))
			INIT_LIST_HEAD(&intf->waiting_rcv_msgs);
		else
			spin_unlock(&intf->waiting_rcv_msgs_lock);

		intf->run_to_completion = 1;
		if (intf->handlers->set_run_to_completion)
			intf->handlers->set_run_to_completion(intf->send_info,
							      1);

		list_for_each_entry_rcu(user, &intf->users, link) {
			if (user->handler->ipmi_panic_handler)
				user->handler->ipmi_panic_handler(
					user->handler_data);
		}

		send_panic_events(intf, ptr);
	}

	return NOTIFY_DONE;
}