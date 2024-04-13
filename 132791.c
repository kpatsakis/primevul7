static int __init ipmi_init_msghandler_mod(void)
{
	int rv;

	pr_info("version " IPMI_DRIVER_VERSION "\n");

	mutex_lock(&ipmi_interfaces_mutex);
	rv = ipmi_register_driver();
	mutex_unlock(&ipmi_interfaces_mutex);

	return rv;
}