vhost_user_slave_config_change(struct virtio_net *dev, bool need_reply)
{
	int ret;
	struct VhostUserMsg msg = {
		.request.slave = VHOST_USER_SLAVE_CONFIG_CHANGE_MSG,
		.flags = VHOST_USER_VERSION,
		.size = 0,
	};

	if (need_reply)
		msg.flags |= VHOST_USER_NEED_REPLY;

	ret = send_vhost_slave_message(dev, &msg);
	if (ret < 0) {
		VHOST_LOG_CONFIG(ERR,
				"Failed to send config change (%d)\n",
				ret);
		return ret;
	}

	return process_slave_message_reply(dev, &msg);
}