static void guid_handler(struct ipmi_smi *intf, struct ipmi_recv_msg *msg)
{
	struct bmc_device *bmc = intf->bmc;

	if ((msg->addr.addr_type != IPMI_SYSTEM_INTERFACE_ADDR_TYPE)
	    || (msg->msg.netfn != IPMI_NETFN_APP_RESPONSE)
	    || (msg->msg.cmd != IPMI_GET_DEVICE_GUID_CMD))
		/* Not for me */
		return;

	if (msg->msg.data[0] != 0) {
		/* Error from getting the GUID, the BMC doesn't have one. */
		bmc->dyn_guid_set = 0;
		goto out;
	}

	if (msg->msg.data_len < UUID_SIZE + 1) {
		bmc->dyn_guid_set = 0;
		dev_warn(intf->si_dev,
			 "The GUID response from the BMC was too short, it was %d but should have been %d.  Assuming GUID is not available.\n",
			 msg->msg.data_len, UUID_SIZE + 1);
		goto out;
	}

	guid_copy(&bmc->fetch_guid, (guid_t *)(msg->msg.data + 1));
	/*
	 * Make sure the guid data is available before setting
	 * dyn_guid_set.
	 */
	smp_wmb();
	bmc->dyn_guid_set = 1;
 out:
	wake_up(&intf->waitq);
}