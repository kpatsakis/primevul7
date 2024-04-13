static void smi_add_watch(struct ipmi_smi *intf, unsigned int flags)
{
	unsigned long iflags;

	if (!intf->handlers->set_need_watch)
		return;

	spin_lock_irqsave(&intf->watch_lock, iflags);
	if (flags & IPMI_WATCH_MASK_CHECK_MESSAGES)
		intf->response_waiters++;

	if (flags & IPMI_WATCH_MASK_CHECK_WATCHDOG)
		intf->watchdog_waiters++;

	if (flags & IPMI_WATCH_MASK_CHECK_COMMANDS)
		intf->command_waiters++;

	if ((intf->last_watch_mask & flags) != flags) {
		intf->last_watch_mask |= flags;
		intf->handlers->set_need_watch(intf->send_info,
					       intf->last_watch_mask);
	}
	spin_unlock_irqrestore(&intf->watch_lock, iflags);
}