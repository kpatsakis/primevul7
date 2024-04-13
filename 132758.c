int ipmi_request_settime(struct ipmi_user *user,
			 struct ipmi_addr *addr,
			 long             msgid,
			 struct kernel_ipmi_msg  *msg,
			 void             *user_msg_data,
			 int              priority,
			 int              retries,
			 unsigned int     retry_time_ms)
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
				    NULL, NULL,
				    priority,
				    saddr,
				    lun,
				    retries,
				    retry_time_ms);

	release_ipmi_user(user, index);
	return rv;
}