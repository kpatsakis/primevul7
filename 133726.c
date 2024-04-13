
static void netif_reset_xps_queues(struct net_device *dev, u16 offset,
				   u16 count)
{
	const unsigned long *possible_mask = NULL;
	struct xps_dev_maps *dev_maps;
	unsigned int nr_ids;

	if (!static_key_false(&xps_needed))
		return;

	cpus_read_lock();
	mutex_lock(&xps_map_mutex);

	if (static_key_false(&xps_rxqs_needed)) {
		dev_maps = xmap_dereference(dev->xps_rxqs_map);
		if (dev_maps) {
			nr_ids = dev->num_rx_queues;
			clean_xps_maps(dev, possible_mask, dev_maps, nr_ids,
				       offset, count, true);
		}
	}

	dev_maps = xmap_dereference(dev->xps_cpus_map);
	if (!dev_maps)
		goto out_no_maps;

	if (num_possible_cpus() > 1)
		possible_mask = cpumask_bits(cpu_possible_mask);
	nr_ids = nr_cpu_ids;
	clean_xps_maps(dev, possible_mask, dev_maps, nr_ids, offset, count,
		       false);

out_no_maps:
	mutex_unlock(&xps_map_mutex);
	cpus_read_unlock();