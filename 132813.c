static void __exit cleanup_ipmi(void)
{
	int count;

	if (initialized) {
		atomic_notifier_chain_unregister(&panic_notifier_list,
						 &panic_block);

		/*
		 * This can't be called if any interfaces exist, so no worry
		 * about shutting down the interfaces.
		 */

		/*
		 * Tell the timer to stop, then wait for it to stop.  This
		 * avoids problems with race conditions removing the timer
		 * here.
		 */
		atomic_set(&stop_operation, 1);
		del_timer_sync(&ipmi_timer);

		initialized = false;

		/* Check for buffer leaks. */
		count = atomic_read(&smi_msg_inuse_count);
		if (count != 0)
			pr_warn("SMI message count %d at exit\n", count);
		count = atomic_read(&recv_msg_inuse_count);
		if (count != 0)
			pr_warn("recv message count %d at exit\n", count);

		cleanup_srcu_struct(&ipmi_interfaces_srcu);
	}
	if (drvregistered)
		driver_unregister(&ipmidriver.driver);
}