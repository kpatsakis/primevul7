int ipmi_set_my_address(struct ipmi_user *user,
			unsigned int  channel,
			unsigned char address)
{
	int index, rv = 0;

	user = acquire_ipmi_user(user, &index);
	if (!user)
		return -ENODEV;

	if (channel >= IPMI_MAX_CHANNELS) {
		rv = -EINVAL;
	} else {
		channel = array_index_nospec(channel, IPMI_MAX_CHANNELS);
		user->intf->addrinfo[channel].address = address;
	}
	release_ipmi_user(user, index);

	return rv;
}