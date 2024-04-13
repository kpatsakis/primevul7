void ipmi_unregister_smi(struct ipmi_smi *intf)
{
	struct ipmi_smi_watcher *w;
	int intf_num = intf->intf_num, index;

	mutex_lock(&ipmi_interfaces_mutex);
	intf->intf_num = -1;
	intf->in_shutdown = true;
	list_del_rcu(&intf->link);
	mutex_unlock(&ipmi_interfaces_mutex);
	synchronize_srcu(&ipmi_interfaces_srcu);

	/* At this point no users can be added to the interface. */

	/*
	 * Call all the watcher interfaces to tell them that
	 * an interface is going away.
	 */
	mutex_lock(&smi_watchers_mutex);
	list_for_each_entry(w, &smi_watchers, link)
		w->smi_gone(intf_num);
	mutex_unlock(&smi_watchers_mutex);

	index = srcu_read_lock(&intf->users_srcu);
	while (!list_empty(&intf->users)) {
		struct ipmi_user *user =
			container_of(list_next_rcu(&intf->users),
				     struct ipmi_user, link);

		_ipmi_destroy_user(user);
	}
	srcu_read_unlock(&intf->users_srcu, index);

	if (intf->handlers->shutdown)
		intf->handlers->shutdown(intf->send_info);

	cleanup_smi_msgs(intf);

	ipmi_bmc_unregister(intf);

	cleanup_srcu_struct(&intf->users_srcu);
	kref_put(&intf->refcount, intf_free);
}