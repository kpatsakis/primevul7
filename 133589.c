
static struct xps_map *expand_xps_map(struct xps_map *map, int attr_index,
				      u16 index, bool is_rxqs_map)
{
	struct xps_map *new_map;
	int alloc_len = XPS_MIN_MAP_ALLOC;
	int i, pos;

	for (pos = 0; map && pos < map->len; pos++) {
		if (map->queues[pos] != index)
			continue;
		return map;
	}

	/* Need to add tx-queue to this CPU's/rx-queue's existing map */
	if (map) {
		if (pos < map->alloc_len)
			return map;

		alloc_len = map->alloc_len * 2;
	}

	/* Need to allocate new map to store tx-queue on this CPU's/rx-queue's
	 *  map
	 */
	if (is_rxqs_map)
		new_map = kzalloc(XPS_MAP_SIZE(alloc_len), GFP_KERNEL);
	else
		new_map = kzalloc_node(XPS_MAP_SIZE(alloc_len), GFP_KERNEL,
				       cpu_to_node(attr_index));
	if (!new_map)
		return NULL;

	for (i = 0; i < pos; i++)
		new_map->queues[i] = map->queues[i];
	new_map->alloc_len = alloc_len;
	new_map->len = pos;

	return new_map;