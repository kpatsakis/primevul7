
static void clean_xps_maps(struct net_device *dev, const unsigned long *mask,
			   struct xps_dev_maps *dev_maps, unsigned int nr_ids,
			   u16 offset, u16 count, bool is_rxqs_map)
{
	bool active = false;
	int i, j;

	for (j = -1; j = netif_attrmask_next(j, mask, nr_ids),
	     j < nr_ids;)
		active |= remove_xps_queue_cpu(dev, dev_maps, j, offset,
					       count);
	if (!active)
		reset_xps_maps(dev, dev_maps, is_rxqs_map);

	if (!is_rxqs_map) {
		for (i = offset + (count - 1); count--; i--) {
			netdev_queue_numa_node_write(
				netdev_get_tx_queue(dev, i),
				NUMA_NO_NODE);
		}
	}