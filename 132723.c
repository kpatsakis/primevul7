static bool ipmi_timeout_handler(struct ipmi_smi *intf,
				 unsigned long timeout_period)
{
	struct list_head     timeouts;
	struct ipmi_recv_msg *msg, *msg2;
	unsigned long        flags;
	int                  i;
	bool                 need_timer = false;

	if (!intf->bmc_registered) {
		kref_get(&intf->refcount);
		if (!schedule_work(&intf->bmc_reg_work)) {
			kref_put(&intf->refcount, intf_free);
			need_timer = true;
		}
	}

	/*
	 * Go through the seq table and find any messages that
	 * have timed out, putting them in the timeouts
	 * list.
	 */
	INIT_LIST_HEAD(&timeouts);
	spin_lock_irqsave(&intf->seq_lock, flags);
	if (intf->ipmb_maintenance_mode_timeout) {
		if (intf->ipmb_maintenance_mode_timeout <= timeout_period)
			intf->ipmb_maintenance_mode_timeout = 0;
		else
			intf->ipmb_maintenance_mode_timeout -= timeout_period;
	}
	for (i = 0; i < IPMI_IPMB_NUM_SEQ; i++)
		check_msg_timeout(intf, &intf->seq_table[i],
				  &timeouts, timeout_period, i,
				  &flags, &need_timer);
	spin_unlock_irqrestore(&intf->seq_lock, flags);

	list_for_each_entry_safe(msg, msg2, &timeouts, link)
		deliver_err_response(intf, msg, IPMI_TIMEOUT_COMPLETION_CODE);

	/*
	 * Maintenance mode handling.  Check the timeout
	 * optimistically before we claim the lock.  It may
	 * mean a timeout gets missed occasionally, but that
	 * only means the timeout gets extended by one period
	 * in that case.  No big deal, and it avoids the lock
	 * most of the time.
	 */
	if (intf->auto_maintenance_timeout > 0) {
		spin_lock_irqsave(&intf->maintenance_mode_lock, flags);
		if (intf->auto_maintenance_timeout > 0) {
			intf->auto_maintenance_timeout
				-= timeout_period;
			if (!intf->maintenance_mode
			    && (intf->auto_maintenance_timeout <= 0)) {
				intf->maintenance_mode_enable = false;
				maintenance_mode_update(intf);
			}
		}
		spin_unlock_irqrestore(&intf->maintenance_mode_lock,
				       flags);
	}

	tasklet_schedule(&intf->recv_tasklet);

	return need_timer;
}