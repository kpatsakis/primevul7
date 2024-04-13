static int msg_msg_alloc_security(struct msg_msg *msg)
{
	struct msg_security_struct *msec;

	msec = kzalloc(sizeof(struct msg_security_struct), GFP_KERNEL);
	if (!msec)
		return -ENOMEM;

	msec->sid = SECINITSID_UNLABELED;
	msg->security = msec;

	return 0;
}