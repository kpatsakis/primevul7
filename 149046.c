vhost_user_set_protocol_features(struct virtio_net **pdev,
			struct VhostUserMsg *msg,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
	uint64_t protocol_features = msg->payload.u64;
	uint64_t slave_protocol_features = 0;

	if (validate_msg_fds(msg, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	rte_vhost_driver_get_protocol_features(dev->ifname,
			&slave_protocol_features);
	if (protocol_features & ~slave_protocol_features) {
		VHOST_LOG_CONFIG(ERR,
			"(%d) received invalid protocol features.\n",
			dev->vid);
		return RTE_VHOST_MSG_RESULT_ERR;
	}

	dev->protocol_features = protocol_features;
	VHOST_LOG_CONFIG(INFO,
		"negotiated Vhost-user protocol features: 0x%" PRIx64 "\n",
		dev->protocol_features);

	return RTE_VHOST_MSG_RESULT_OK;
}