read_vhost_message(int sockfd, struct VhostUserMsg *msg)
{
	int ret;

	ret = read_fd_message(sockfd, (char *)msg, VHOST_USER_HDR_SIZE,
		msg->fds, VHOST_MEMORY_MAX_NREGIONS, &msg->fd_num);
	if (ret <= 0) {
		return ret;
	} else if (ret != VHOST_USER_HDR_SIZE) {
		VHOST_LOG_CONFIG(ERR, "Unexpected header size read\n");
		close_msg_fds(msg);
		return -1;
	}

	if (msg->size) {
		if (msg->size > sizeof(msg->payload)) {
			VHOST_LOG_CONFIG(ERR,
				"invalid msg size: %d\n", msg->size);
			return -1;
		}
		ret = read(sockfd, &msg->payload, msg->size);
		if (ret <= 0)
			return ret;
		if (ret != (int)msg->size) {
			VHOST_LOG_CONFIG(ERR,
				"read control message failed\n");
			return -1;
		}
	}

	return ret;
}