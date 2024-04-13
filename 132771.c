static int i_ipmi_req_sysintf(struct ipmi_smi        *intf,
			      struct ipmi_addr       *addr,
			      long                   msgid,
			      struct kernel_ipmi_msg *msg,
			      struct ipmi_smi_msg    *smi_msg,
			      struct ipmi_recv_msg   *recv_msg,
			      int                    retries,
			      unsigned int           retry_time_ms)
{
	struct ipmi_system_interface_addr *smi_addr;

	if (msg->netfn & 1)
		/* Responses are not allowed to the SMI. */
		return -EINVAL;

	smi_addr = (struct ipmi_system_interface_addr *) addr;
	if (smi_addr->lun > 3) {
		ipmi_inc_stat(intf, sent_invalid_commands);
		return -EINVAL;
	}

	memcpy(&recv_msg->addr, smi_addr, sizeof(*smi_addr));

	if ((msg->netfn == IPMI_NETFN_APP_REQUEST)
	    && ((msg->cmd == IPMI_SEND_MSG_CMD)
		|| (msg->cmd == IPMI_GET_MSG_CMD)
		|| (msg->cmd == IPMI_READ_EVENT_MSG_BUFFER_CMD))) {
		/*
		 * We don't let the user do these, since we manage
		 * the sequence numbers.
		 */
		ipmi_inc_stat(intf, sent_invalid_commands);
		return -EINVAL;
	}

	if (is_maintenance_mode_cmd(msg)) {
		unsigned long flags;

		spin_lock_irqsave(&intf->maintenance_mode_lock, flags);
		intf->auto_maintenance_timeout
			= maintenance_mode_timeout_ms;
		if (!intf->maintenance_mode
		    && !intf->maintenance_mode_enable) {
			intf->maintenance_mode_enable = true;
			maintenance_mode_update(intf);
		}
		spin_unlock_irqrestore(&intf->maintenance_mode_lock,
				       flags);
	}

	if (msg->data_len + 2 > IPMI_MAX_MSG_LENGTH) {
		ipmi_inc_stat(intf, sent_invalid_commands);
		return -EMSGSIZE;
	}

	smi_msg->data[0] = (msg->netfn << 2) | (smi_addr->lun & 0x3);
	smi_msg->data[1] = msg->cmd;
	smi_msg->msgid = msgid;
	smi_msg->user_data = recv_msg;
	if (msg->data_len > 0)
		memcpy(&smi_msg->data[2], msg->data, msg->data_len);
	smi_msg->data_size = msg->data_len + 2;
	ipmi_inc_stat(intf, sent_local_commands);

	return 0;
}