int ipmi_create_user(unsigned int          if_num,
		     const struct ipmi_user_hndl *handler,
		     void                  *handler_data,
		     struct ipmi_user      **user)
{
	unsigned long flags;
	struct ipmi_user *new_user;
	int           rv, index;
	struct ipmi_smi *intf;

	/*
	 * There is no module usecount here, because it's not
	 * required.  Since this can only be used by and called from
	 * other modules, they will implicitly use this module, and
	 * thus this can't be removed unless the other modules are
	 * removed.
	 */

	if (handler == NULL)
		return -EINVAL;

	/*
	 * Make sure the driver is actually initialized, this handles
	 * problems with initialization order.
	 */
	rv = ipmi_init_msghandler();
	if (rv)
		return rv;

	new_user = kmalloc(sizeof(*new_user), GFP_KERNEL);
	if (!new_user)
		return -ENOMEM;

	index = srcu_read_lock(&ipmi_interfaces_srcu);
	list_for_each_entry_rcu(intf, &ipmi_interfaces, link) {
		if (intf->intf_num == if_num)
			goto found;
	}
	/* Not found, return an error */
	rv = -EINVAL;
	goto out_kfree;

 found:
	INIT_WORK(&new_user->remove_work, free_user_work);

	rv = init_srcu_struct(&new_user->release_barrier);
	if (rv)
		goto out_kfree;

	if (!try_module_get(intf->owner)) {
		rv = -ENODEV;
		goto out_kfree;
	}

	/* Note that each existing user holds a refcount to the interface. */
	kref_get(&intf->refcount);

	kref_init(&new_user->refcount);
	new_user->handler = handler;
	new_user->handler_data = handler_data;
	new_user->intf = intf;
	new_user->gets_events = false;

	rcu_assign_pointer(new_user->self, new_user);
	spin_lock_irqsave(&intf->seq_lock, flags);
	list_add_rcu(&new_user->link, &intf->users);
	spin_unlock_irqrestore(&intf->seq_lock, flags);
	if (handler->ipmi_watchdog_pretimeout)
		/* User wants pretimeouts, so make sure to watch for them. */
		smi_add_watch(intf, IPMI_WATCH_MASK_CHECK_WATCHDOG);
	srcu_read_unlock(&ipmi_interfaces_srcu, index);
	*user = new_user;
	return 0;

out_kfree:
	srcu_read_unlock(&ipmi_interfaces_srcu, index);
	kfree(new_user);
	return rv;
}