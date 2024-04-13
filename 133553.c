#ifdef CONFIG_XPS
static int __get_xps_queue_idx(struct net_device *dev, struct sk_buff *skb,
			       struct xps_dev_maps *dev_maps, unsigned int tci)
{
	struct xps_map *map;
	int queue_index = -1;

	if (dev->num_tc) {
		tci *= dev->num_tc;
		tci += netdev_get_prio_tc_map(dev, skb->priority);
	}

	map = rcu_dereference(dev_maps->attr_map[tci]);
	if (map) {
		if (map->len == 1)
			queue_index = map->queues[0];
		else
			queue_index = map->queues[reciprocal_scale(
						skb_get_hash(skb), map->len)];
		if (unlikely(queue_index >= dev->real_num_tx_queues))
			queue_index = -1;
	}
	return queue_index;