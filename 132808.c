static void smi_recv_tasklet(unsigned long val)
{
	unsigned long flags = 0; /* keep us warning-free. */
	struct ipmi_smi *intf = (struct ipmi_smi *) val;
	int run_to_completion = intf->run_to_completion;
	struct ipmi_smi_msg *newmsg = NULL;

	/*
	 * Start the next message if available.
	 *
	 * Do this here, not in the actual receiver, because we may deadlock
	 * because the lower layer is allowed to hold locks while calling
	 * message delivery.
	 */

	rcu_read_lock();

	if (!run_to_completion)
		spin_lock_irqsave(&intf->xmit_msgs_lock, flags);
	if (intf->curr_msg == NULL && !intf->in_shutdown) {
		struct list_head *entry = NULL;

		/* Pick the high priority queue first. */
		if (!list_empty(&intf->hp_xmit_msgs))
			entry = intf->hp_xmit_msgs.next;
		else if (!list_empty(&intf->xmit_msgs))
			entry = intf->xmit_msgs.next;

		if (entry) {
			list_del(entry);
			newmsg = list_entry(entry, struct ipmi_smi_msg, link);
			intf->curr_msg = newmsg;
		}
	}

	if (!run_to_completion)
		spin_unlock_irqrestore(&intf->xmit_msgs_lock, flags);
	if (newmsg)
		intf->handlers->sender(intf->send_info, newmsg);

	rcu_read_unlock();

	handle_new_recv_msgs(intf);
}