int ipmi_request_supply_msgs(struct ipmi_user     *user,
			     struct ipmi_addr     *addr,
			     long                 msgid,
			     struct kernel_ipmi_msg *msg,
			     void                 *user_msg_data,
			     void                 *supplied_smi,
			     struct ipmi_recv_msg *supplied_recv,
			     int                  priority)
{
	unsigned char saddr = 0, lun = 0;
	int rv, index;

	if (!user)
		return -EINVAL;

	user = acquire_ipmi_user(user, &index);
	if (!user)
		return -ENODEV;

	rv = check_addr(user->intf, addr, &saddr, &lun);
	if (!rv)
		rv = i_ipmi_request(user,
				    user->intf,
				    addr,
				    msgid,
				    msg,
				    user_msg_data,
				    supplied_smi,
				    supplied_recv,
				    priority,
				    saddr,
				    lun,
				    -1, 0);

	release_ipmi_user(user, index);
	return rv;
}