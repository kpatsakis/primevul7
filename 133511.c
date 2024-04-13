
static void __net_exit default_device_exit_batch(struct list_head *net_list)
{
	/* At exit all network devices most be removed from a network
	 * namespace.  Do this in the reverse order of registration.
	 * Do this across as many network namespaces as possible to
	 * improve batching efficiency.
	 */
	struct net_device *dev;
	struct net *net;
	LIST_HEAD(dev_kill_list);

	/* To prevent network device cleanup code from dereferencing
	 * loopback devices or network devices that have been freed
	 * wait here for all pending unregistrations to complete,
	 * before unregistring the loopback device and allowing the
	 * network namespace be freed.
	 *
	 * The netdev todo list containing all network devices
	 * unregistrations that happen in default_device_exit_batch
	 * will run in the rtnl_unlock() at the end of
	 * default_device_exit_batch.
	 */
	rtnl_lock_unregistering(net_list);
	list_for_each_entry(net, net_list, exit_list) {
		for_each_netdev_reverse(net, dev) {
			if (dev->rtnl_link_ops && dev->rtnl_link_ops->dellink)
				dev->rtnl_link_ops->dellink(dev, &dev_kill_list);
			else
				unregister_netdevice_queue(dev, &dev_kill_list);
		}
	}
	unregister_netdevice_many(&dev_kill_list);
	rtnl_unlock();