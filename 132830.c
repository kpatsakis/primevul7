static int handle_bmc_rsp(struct ipmi_smi *intf,
			  struct ipmi_smi_msg *msg)
{
	struct ipmi_recv_msg *recv_msg;
	struct ipmi_system_interface_addr *smi_addr;

	recv_msg = (struct ipmi_recv_msg *) msg->user_data;
	if (recv_msg == NULL) {
		dev_warn(intf->si_dev,
			 "IPMI message received with no owner. This could be because of a malformed message, or because of a hardware error.  Contact your hardware vendor for assistance.\n");
		return 0;
	}

	recv_msg->recv_type = IPMI_RESPONSE_RECV_TYPE;
	recv_msg->msgid = msg->msgid;
	smi_addr = ((struct ipmi_system_interface_addr *)
		    &recv_msg->addr);
	smi_addr->addr_type = IPMI_SYSTEM_INTERFACE_ADDR_TYPE;
	smi_addr->channel = IPMI_BMC_CHANNEL;
	smi_addr->lun = msg->rsp[0] & 3;
	recv_msg->msg.netfn = msg->rsp[0] >> 2;
	recv_msg->msg.cmd = msg->rsp[1];
	memcpy(recv_msg->msg_data, &msg->rsp[2], msg->rsp_size - 2);
	recv_msg->msg.data = recv_msg->msg_data;
	recv_msg->msg.data_len = msg->rsp_size - 2;
	deliver_local_response(intf, recv_msg);

	return 0;
}