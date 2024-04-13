
int dev_set_mtu(struct net_device *dev, int new_mtu)
{
	struct netlink_ext_ack extack;
	int err;

	memset(&extack, 0, sizeof(extack));
	err = dev_set_mtu_ext(dev, new_mtu, &extack);
	if (err && extack._msg)
		net_err_ratelimited("%s: %s\n", dev->name, extack._msg);
	return err;