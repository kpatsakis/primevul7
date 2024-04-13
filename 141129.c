static void msg_msg_free_security(struct msg_msg *msg)
{
	struct msg_security_struct *msec = msg->security;

	msg->security = NULL;
	kfree(msec);
}