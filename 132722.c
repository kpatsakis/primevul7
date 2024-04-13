static void need_waiter(struct ipmi_smi *intf)
{
	/* Racy, but worst case we start the timer twice. */
	if (!timer_pending(&ipmi_timer))
		mod_timer(&ipmi_timer, jiffies + IPMI_TIMEOUT_JIFFIES);
}