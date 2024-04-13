static int __get_device_id(struct ipmi_smi *intf, struct bmc_device *bmc)
{
	int rv;

	bmc->dyn_id_set = 2;

	intf->null_user_handler = bmc_device_id_handler;

	rv = send_get_device_id_cmd(intf);
	if (rv)
		return rv;

	wait_event(intf->waitq, bmc->dyn_id_set != 2);

	if (!bmc->dyn_id_set)
		rv = -EIO; /* Something went wrong in the fetch. */

	/* dyn_id_set makes the id data available. */
	smp_rmb();

	intf->null_user_handler = NULL;

	return rv;
}