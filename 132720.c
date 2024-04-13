static void deliver_err_response(struct ipmi_smi *intf,
				 struct ipmi_recv_msg *msg, int err)
{
	msg->recv_type = IPMI_RESPONSE_RECV_TYPE;
	msg->msg_data[0] = err;
	msg->msg.netfn |= 1; /* Convert to a response. */
	msg->msg.data_len = 1;
	msg->msg.data = msg->msg_data;
	deliver_local_response(intf, msg);
}