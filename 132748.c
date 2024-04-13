static void deliver_local_response(struct ipmi_smi *intf,
				   struct ipmi_recv_msg *msg)
{
	if (deliver_response(intf, msg))
		ipmi_inc_stat(intf, unhandled_local_responses);
	else
		ipmi_inc_stat(intf, handled_local_responses);
}