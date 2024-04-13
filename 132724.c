void ipmi_poll_interface(struct ipmi_user *user)
{
	ipmi_poll(user->intf);
}