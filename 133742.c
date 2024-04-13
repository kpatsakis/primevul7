
static void netif_reset_xps_queues_gt(struct net_device *dev, u16 index)
{
	netif_reset_xps_queues(dev, index, dev->num_tx_queues - index);