close_msg_fds(struct VhostUserMsg *msg)
{
	int i;

	for (i = 0; i < msg->fd_num; i++)
		close(msg->fds[i]);
}