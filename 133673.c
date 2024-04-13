
static void netif_free_rx_queues(struct net_device *dev)
{
	unsigned int i, count = dev->num_rx_queues;

	/* netif_alloc_rx_queues alloc failed, resources have been unreg'ed */
	if (!dev->_rx)
		return;

	for (i = 0; i < count; i++)
		xdp_rxq_info_unreg(&dev->_rx[i].xdp_rxq);

	kvfree(dev->_rx);