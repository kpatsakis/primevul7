
static ssize_t xps_rxqs_store(struct netdev_queue *queue, const char *buf,
			      size_t len)
{
	struct net_device *dev = queue->dev;
	struct net *net = dev_net(dev);
	unsigned long *mask, index;
	int err;

	if (!ns_capable(net->user_ns, CAP_NET_ADMIN))
		return -EPERM;

	mask = kcalloc(BITS_TO_LONGS(dev->num_rx_queues), sizeof(long),
		       GFP_KERNEL);
	if (!mask)
		return -ENOMEM;

	index = get_netdev_queue_index(queue);

	err = bitmap_parse(buf, len, mask, dev->num_rx_queues);
	if (err) {
		kfree(mask);
		return err;
	}

	cpus_read_lock();
	err = __netif_set_xps_queue(dev, mask, index, true);
	cpus_read_unlock();

	kfree(mask);
	return err ? : len;