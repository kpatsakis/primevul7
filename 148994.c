rte_vhost_slave_config_change(int vid, bool need_reply)
{
	struct virtio_net *dev;

	dev = get_device(vid);
	if (!dev)
		return -ENODEV;

	return vhost_user_slave_config_change(dev, need_reply);
}