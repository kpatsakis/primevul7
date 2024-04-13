validate_msg_fds(struct VhostUserMsg *msg, int expected_fds)
{
	if (msg->fd_num == expected_fds)
		return 0;

	VHOST_LOG_CONFIG(ERR,
		" Expect %d FDs for request %s, received %d\n",
		expected_fds,
		vhost_message_str[msg->request.master],
		msg->fd_num);

	close_msg_fds(msg);

	return -1;
}