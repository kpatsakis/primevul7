vhost_user_reset_owner(struct virtio_net **pdev,
			struct VhostUserMsg *msg,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;

	if (validate_msg_fds(msg, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	vhost_destroy_device_notify(dev);

	cleanup_device(dev, 0);
	reset_device(dev);
	return RTE_VHOST_MSG_RESULT_OK;
}