
static bool remove_xps_queue_cpu(struct net_device *dev,
				 struct xps_dev_maps *dev_maps,
				 int cpu, u16 offset, u16 count)
{
	int num_tc = dev->num_tc ? : 1;
	bool active = false;
	int tci;

	for (tci = cpu * num_tc; num_tc--; tci++) {
		int i, j;

		for (i = count, j = offset; i--; j++) {
			if (!remove_xps_queue(dev_maps, tci, j))
				break;
		}

		active |= i < 0;
	}

	return active;