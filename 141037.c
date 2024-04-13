static void selinux_msg_msg_free_security(struct msg_msg *msg)
{
	msg_msg_free_security(msg);
}