
int netif_set_xps_queue(struct net_device *dev, const struct cpumask *mask,
			u16 index)
{
	int ret;

	cpus_read_lock();
	ret =  __netif_set_xps_queue(dev, cpumask_bits(mask), index, false);
	cpus_read_unlock();

	return ret;