static int ipmi_init_msghandler(void)
{
	int rv;

	mutex_lock(&ipmi_interfaces_mutex);
	rv = ipmi_register_driver();
	if (rv)
		goto out;
	if (initialized)
		goto out;

	init_srcu_struct(&ipmi_interfaces_srcu);

	timer_setup(&ipmi_timer, ipmi_timeout, 0);
	mod_timer(&ipmi_timer, jiffies + IPMI_TIMEOUT_JIFFIES);

	atomic_notifier_chain_register(&panic_notifier_list, &panic_block);

	initialized = true;

out:
	mutex_unlock(&ipmi_interfaces_mutex);
	return rv;
}