
static int get_xps_queue(struct net_device *dev, struct net_device *sb_dev,
			 struct sk_buff *skb)
{
#ifdef CONFIG_XPS
	struct xps_dev_maps *dev_maps;
	struct sock *sk = skb->sk;
	int queue_index = -1;

	if (!static_key_false(&xps_needed))
		return -1;

	rcu_read_lock();
	if (!static_key_false(&xps_rxqs_needed))
		goto get_cpus_map;

	dev_maps = rcu_dereference(sb_dev->xps_rxqs_map);
	if (dev_maps) {
		int tci = sk_rx_queue_get(sk);

		if (tci >= 0 && tci < dev->num_rx_queues)
			queue_index = __get_xps_queue_idx(dev, skb, dev_maps,
							  tci);
	}

get_cpus_map:
	if (queue_index < 0) {
		dev_maps = rcu_dereference(sb_dev->xps_cpus_map);
		if (dev_maps) {
			unsigned int tci = skb->sender_cpu - 1;

			queue_index = __get_xps_queue_idx(dev, skb, dev_maps,
							  tci);
		}
	}
	rcu_read_unlock();

	return queue_index;
#else
	return -1;
#endif