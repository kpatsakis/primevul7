static void bmc_device_id_handler(struct ipmi_smi *intf,
				  struct ipmi_recv_msg *msg)
{
	int rv;

	if ((msg->addr.addr_type != IPMI_SYSTEM_INTERFACE_ADDR_TYPE)
			|| (msg->msg.netfn != IPMI_NETFN_APP_RESPONSE)
			|| (msg->msg.cmd != IPMI_GET_DEVICE_ID_CMD)) {
		dev_warn(intf->si_dev,
			 "invalid device_id msg: addr_type=%d netfn=%x cmd=%x\n",
			 msg->addr.addr_type, msg->msg.netfn, msg->msg.cmd);
		return;
	}

	rv = ipmi_demangle_device_id(msg->msg.netfn, msg->msg.cmd,
			msg->msg.data, msg->msg.data_len, &intf->bmc->fetch_id);
	if (rv) {
		dev_warn(intf->si_dev, "device id demangle failed: %d\n", rv);
		intf->bmc->dyn_id_set = 0;
	} else {
		/*
		 * Make sure the id data is available before setting
		 * dyn_id_set.
		 */
		smp_wmb();
		intf->bmc->dyn_id_set = 1;
	}

	wake_up(&intf->waitq);
}