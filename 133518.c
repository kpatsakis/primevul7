
static void reset_xps_maps(struct net_device *dev,
			   struct xps_dev_maps *dev_maps,
			   bool is_rxqs_map)
{
	if (is_rxqs_map) {
		static_key_slow_dec_cpuslocked(&xps_rxqs_needed);
		RCU_INIT_POINTER(dev->xps_rxqs_map, NULL);
	} else {
		RCU_INIT_POINTER(dev->xps_cpus_map, NULL);
	}
	static_key_slow_dec_cpuslocked(&xps_needed);
	kfree_rcu(dev_maps, rcu);