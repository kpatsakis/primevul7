static void ipmi_bmc_unregister(struct ipmi_smi *intf)
{
	mutex_lock(&intf->bmc_reg_mutex);
	__ipmi_bmc_unregister(intf);
	mutex_unlock(&intf->bmc_reg_mutex);
}