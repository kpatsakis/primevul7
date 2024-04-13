static bool is_maintenance_mode_cmd(struct kernel_ipmi_msg *msg)
{
	return (((msg->netfn == IPMI_NETFN_APP_REQUEST)
		 && ((msg->cmd == IPMI_COLD_RESET_CMD)
		     || (msg->cmd == IPMI_WARM_RESET_CMD)))
		|| (msg->netfn == IPMI_NETFN_FIRMWARE_REQUEST));
}