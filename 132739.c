static void ipmi_timeout(struct timer_list *unused)
{
	struct ipmi_smi *intf;
	bool need_timer = false;
	int index;

	if (atomic_read(&stop_operation))
		return;

	index = srcu_read_lock(&ipmi_interfaces_srcu);
	list_for_each_entry_rcu(intf, &ipmi_interfaces, link) {
		if (atomic_read(&intf->event_waiters)) {
			intf->ticks_to_req_ev--;
			if (intf->ticks_to_req_ev == 0) {
				ipmi_request_event(intf);
				intf->ticks_to_req_ev = IPMI_REQUEST_EV_TIME;
			}
			need_timer = true;
		}

		need_timer |= ipmi_timeout_handler(intf, IPMI_TIMEOUT_TIME);
	}
	srcu_read_unlock(&ipmi_interfaces_srcu, index);

	if (need_timer)
		mod_timer(&ipmi_timer, jiffies + IPMI_TIMEOUT_JIFFIES);
}