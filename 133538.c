 */
int register_netdev(struct net_device *dev)
{
	int err;

	if (rtnl_lock_killable())
		return -EINTR;
	err = register_netdevice(dev);
	rtnl_unlock();
	return err;