
static int netif_alloc_rx_queues(struct net_device *dev)
{
	unsigned int i, count = dev->num_rx_queues;
	struct netdev_rx_queue *rx;
	size_t sz = count * sizeof(*rx);
	int err = 0;

	BUG_ON(count < 1);

	rx = kvzalloc(sz, GFP_KERNEL | __GFP_RETRY_MAYFAIL);
	if (!rx)
		return -ENOMEM;

	dev->_rx = rx;

	for (i = 0; i < count; i++) {
		rx[i].dev = dev;

		/* XDP RX-queue setup */
		err = xdp_rxq_info_reg(&rx[i].xdp_rxq, dev, i);
		if (err < 0)
			goto err_rxq_info;
	}
	return 0;

err_rxq_info:
	/* Rollback successful reg's and free other resources */
	while (i--)
		xdp_rxq_info_unreg(&rx[i].xdp_rxq);
	kvfree(dev->_rx);
	dev->_rx = NULL;
	return err;