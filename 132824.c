int ipmi_smi_watcher_register(struct ipmi_smi_watcher *watcher)
{
	struct ipmi_smi *intf;
	int index, rv;

	/*
	 * Make sure the driver is actually initialized, this handles
	 * problems with initialization order.
	 */
	rv = ipmi_init_msghandler();
	if (rv)
		return rv;

	mutex_lock(&smi_watchers_mutex);

	list_add(&watcher->link, &smi_watchers);

	index = srcu_read_lock(&ipmi_interfaces_srcu);
	list_for_each_entry_rcu(intf, &ipmi_interfaces, link) {
		int intf_num = READ_ONCE(intf->intf_num);

		if (intf_num == -1)
			continue;
		watcher->new_smi(intf_num, intf->si_dev);
	}
	srcu_read_unlock(&ipmi_interfaces_srcu, index);

	mutex_unlock(&smi_watchers_mutex);

	return 0;
}