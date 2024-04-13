void ipmi_smi_watchdog_pretimeout(struct ipmi_smi *intf)
{
	if (intf->in_shutdown)
		return;

	atomic_set(&intf->watchdog_pretimeouts_to_deliver, 1);
	tasklet_schedule(&intf->recv_tasklet);
}