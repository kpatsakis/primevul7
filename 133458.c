
int __netdev_update_features(struct net_device *dev)
{
	struct net_device *upper, *lower;
	netdev_features_t features;
	struct list_head *iter;
	int err = -1;

	ASSERT_RTNL();

	features = netdev_get_wanted_features(dev);

	if (dev->netdev_ops->ndo_fix_features)
		features = dev->netdev_ops->ndo_fix_features(dev, features);

	/* driver might be less strict about feature dependencies */
	features = netdev_fix_features(dev, features);

	/* some features can't be enabled if they're off an an upper device */
	netdev_for_each_upper_dev_rcu(dev, upper, iter)
		features = netdev_sync_upper_features(dev, upper, features);

	if (dev->features == features)
		goto sync_lower;

	netdev_dbg(dev, "Features changed: %pNF -> %pNF\n",
		&dev->features, &features);

	if (dev->netdev_ops->ndo_set_features)
		err = dev->netdev_ops->ndo_set_features(dev, features);
	else
		err = 0;

	if (unlikely(err < 0)) {
		netdev_err(dev,
			"set_features() failed (%d); wanted %pNF, left %pNF\n",
			err, &features, &dev->features);
		/* return non-0 since some features might have changed and
		 * it's better to fire a spurious notification than miss it
		 */
		return -1;
	}

sync_lower:
	/* some features must be disabled on lower devices when disabled
	 * on an upper device (think: bonding master or bridge)
	 */
	netdev_for_each_lower_dev(dev, lower, iter)
		netdev_sync_lower_features(dev, lower, features);

	if (!err) {
		netdev_features_t diff = features ^ dev->features;

		if (diff & NETIF_F_RX_UDP_TUNNEL_PORT) {
			/* udp_tunnel_{get,drop}_rx_info both need
			 * NETIF_F_RX_UDP_TUNNEL_PORT enabled on the
			 * device, or they won't do anything.
			 * Thus we need to update dev->features
			 * *before* calling udp_tunnel_get_rx_info,
			 * but *after* calling udp_tunnel_drop_rx_info.
			 */
			if (features & NETIF_F_RX_UDP_TUNNEL_PORT) {
				dev->features = features;
				udp_tunnel_get_rx_info(dev);
			} else {
				udp_tunnel_drop_rx_info(dev);
			}
		}

		if (diff & NETIF_F_HW_VLAN_CTAG_FILTER) {
			if (features & NETIF_F_HW_VLAN_CTAG_FILTER) {
				dev->features = features;
				err |= vlan_get_rx_ctag_filter_info(dev);
			} else {
				vlan_drop_rx_ctag_filter_info(dev);
			}
		}

		if (diff & NETIF_F_HW_VLAN_STAG_FILTER) {
			if (features & NETIF_F_HW_VLAN_STAG_FILTER) {
				dev->features = features;
				err |= vlan_get_rx_stag_filter_info(dev);
			} else {
				vlan_drop_rx_stag_filter_info(dev);
			}
		}

		dev->features = features;
	}

	return err < 0 ? 0 : 1;