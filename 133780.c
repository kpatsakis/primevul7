/* Must be called under cpus_read_lock */
int __netif_set_xps_queue(struct net_device *dev, const unsigned long *mask,
			  u16 index, bool is_rxqs_map)
{
	const unsigned long *online_mask = NULL, *possible_mask = NULL;
	struct xps_dev_maps *dev_maps, *new_dev_maps = NULL;
	int i, j, tci, numa_node_id = -2;
	int maps_sz, num_tc = 1, tc = 0;
	struct xps_map *map, *new_map;
	bool active = false;
	unsigned int nr_ids;

	if (dev->num_tc) {
		/* Do not allow XPS on subordinate device directly */
		num_tc = dev->num_tc;
		if (num_tc < 0)
			return -EINVAL;

		/* If queue belongs to subordinate dev use its map */
		dev = netdev_get_tx_queue(dev, index)->sb_dev ? : dev;

		tc = netdev_txq_to_tc(dev, index);
		if (tc < 0)
			return -EINVAL;
	}

	mutex_lock(&xps_map_mutex);
	if (is_rxqs_map) {
		maps_sz = XPS_RXQ_DEV_MAPS_SIZE(num_tc, dev->num_rx_queues);
		dev_maps = xmap_dereference(dev->xps_rxqs_map);
		nr_ids = dev->num_rx_queues;
	} else {
		maps_sz = XPS_CPU_DEV_MAPS_SIZE(num_tc);
		if (num_possible_cpus() > 1) {
			online_mask = cpumask_bits(cpu_online_mask);
			possible_mask = cpumask_bits(cpu_possible_mask);
		}
		dev_maps = xmap_dereference(dev->xps_cpus_map);
		nr_ids = nr_cpu_ids;
	}

	if (maps_sz < L1_CACHE_BYTES)
		maps_sz = L1_CACHE_BYTES;

	/* allocate memory for queue storage */
	for (j = -1; j = netif_attrmask_next_and(j, online_mask, mask, nr_ids),
	     j < nr_ids;) {
		if (!new_dev_maps)
			new_dev_maps = kzalloc(maps_sz, GFP_KERNEL);
		if (!new_dev_maps) {
			mutex_unlock(&xps_map_mutex);
			return -ENOMEM;
		}

		tci = j * num_tc + tc;
		map = dev_maps ? xmap_dereference(dev_maps->attr_map[tci]) :
				 NULL;

		map = expand_xps_map(map, j, index, is_rxqs_map);
		if (!map)
			goto error;

		RCU_INIT_POINTER(new_dev_maps->attr_map[tci], map);
	}

	if (!new_dev_maps)
		goto out_no_new_maps;

	if (!dev_maps) {
		/* Increment static keys at most once per type */
		static_key_slow_inc_cpuslocked(&xps_needed);
		if (is_rxqs_map)
			static_key_slow_inc_cpuslocked(&xps_rxqs_needed);
	}

	for (j = -1; j = netif_attrmask_next(j, possible_mask, nr_ids),
	     j < nr_ids;) {
		/* copy maps belonging to foreign traffic classes */
		for (i = tc, tci = j * num_tc; dev_maps && i--; tci++) {
			/* fill in the new device map from the old device map */
			map = xmap_dereference(dev_maps->attr_map[tci]);
			RCU_INIT_POINTER(new_dev_maps->attr_map[tci], map);
		}

		/* We need to explicitly update tci as prevous loop
		 * could break out early if dev_maps is NULL.
		 */
		tci = j * num_tc + tc;

		if (netif_attr_test_mask(j, mask, nr_ids) &&
		    netif_attr_test_online(j, online_mask, nr_ids)) {
			/* add tx-queue to CPU/rx-queue maps */
			int pos = 0;

			map = xmap_dereference(new_dev_maps->attr_map[tci]);
			while ((pos < map->len) && (map->queues[pos] != index))
				pos++;

			if (pos == map->len)
				map->queues[map->len++] = index;
#ifdef CONFIG_NUMA
			if (!is_rxqs_map) {
				if (numa_node_id == -2)
					numa_node_id = cpu_to_node(j);
				else if (numa_node_id != cpu_to_node(j))
					numa_node_id = -1;
			}
#endif
		} else if (dev_maps) {
			/* fill in the new device map from the old device map */
			map = xmap_dereference(dev_maps->attr_map[tci]);
			RCU_INIT_POINTER(new_dev_maps->attr_map[tci], map);
		}

		/* copy maps belonging to foreign traffic classes */
		for (i = num_tc - tc, tci++; dev_maps && --i; tci++) {
			/* fill in the new device map from the old device map */
			map = xmap_dereference(dev_maps->attr_map[tci]);
			RCU_INIT_POINTER(new_dev_maps->attr_map[tci], map);
		}
	}

	if (is_rxqs_map)
		rcu_assign_pointer(dev->xps_rxqs_map, new_dev_maps);
	else
		rcu_assign_pointer(dev->xps_cpus_map, new_dev_maps);

	/* Cleanup old maps */
	if (!dev_maps)
		goto out_no_old_maps;

	for (j = -1; j = netif_attrmask_next(j, possible_mask, nr_ids),
	     j < nr_ids;) {
		for (i = num_tc, tci = j * num_tc; i--; tci++) {
			new_map = xmap_dereference(new_dev_maps->attr_map[tci]);
			map = xmap_dereference(dev_maps->attr_map[tci]);
			if (map && map != new_map)
				kfree_rcu(map, rcu);
		}
	}

	kfree_rcu(dev_maps, rcu);

out_no_old_maps:
	dev_maps = new_dev_maps;
	active = true;

out_no_new_maps:
	if (!is_rxqs_map) {
		/* update Tx queue numa node */
		netdev_queue_numa_node_write(netdev_get_tx_queue(dev, index),
					     (numa_node_id >= 0) ?
					     numa_node_id : NUMA_NO_NODE);
	}

	if (!dev_maps)
		goto out_no_maps;

	/* removes tx-queue from unused CPUs/rx-queues */
	for (j = -1; j = netif_attrmask_next(j, possible_mask, nr_ids),
	     j < nr_ids;) {
		for (i = tc, tci = j * num_tc; i--; tci++)
			active |= remove_xps_queue(dev_maps, tci, index);
		if (!netif_attr_test_mask(j, mask, nr_ids) ||
		    !netif_attr_test_online(j, online_mask, nr_ids))
			active |= remove_xps_queue(dev_maps, tci, index);
		for (i = num_tc - tc, tci++; --i; tci++)
			active |= remove_xps_queue(dev_maps, tci, index);
	}

	/* free map if not active */
	if (!active)
		reset_xps_maps(dev, dev_maps, is_rxqs_map);

out_no_maps:
	mutex_unlock(&xps_map_mutex);

	return 0;
error:
	/* remove any maps that we added */
	for (j = -1; j = netif_attrmask_next(j, possible_mask, nr_ids),
	     j < nr_ids;) {
		for (i = num_tc, tci = j * num_tc; i--; tci++) {
			new_map = xmap_dereference(new_dev_maps->attr_map[tci]);
			map = dev_maps ?
			      xmap_dereference(dev_maps->attr_map[tci]) :
			      NULL;
			if (new_map && new_map != map)
				kfree(new_map);
		}
	}

	mutex_unlock(&xps_map_mutex);

	kfree(new_dev_maps);
	return -ENOMEM;