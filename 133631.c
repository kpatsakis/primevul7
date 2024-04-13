
static void netif_free_tx_queues(struct net_device *dev)
{
	kvfree(dev->_tx);