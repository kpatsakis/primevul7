
static bool remove_xps_queue(struct xps_dev_maps *dev_maps,
			     int tci, u16 index)
{
	struct xps_map *map = NULL;
	int pos;

	if (dev_maps)
		map = xmap_dereference(dev_maps->attr_map[tci]);
	if (!map)
		return false;

	for (pos = map->len; pos--;) {
		if (map->queues[pos] != index)
			continue;

		if (map->len > 1) {
			map->queues[pos] = map->queues[--map->len];
			break;
		}

		RCU_INIT_POINTER(dev_maps->attr_map[tci], NULL);
		kfree_rcu(map, rcu);
		return false;
	}

	return true;