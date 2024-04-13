static int bmc_get_device_id(struct ipmi_smi *intf, struct bmc_device *bmc,
			     struct ipmi_device_id *id,
			     bool *guid_set, guid_t *guid)
{
	return __bmc_get_device_id(intf, bmc, id, guid_set, guid, -1);
}