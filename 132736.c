static void redo_bmc_reg(struct work_struct *work)
{
	struct ipmi_smi *intf = container_of(work, struct ipmi_smi,
					     bmc_reg_work);

	if (!intf->in_shutdown)
		bmc_get_device_id(intf, NULL, NULL, NULL, NULL);

	kref_put(&intf->refcount, intf_free);
}