
void __dev_notify_flags(struct net_device *dev, unsigned int old_flags,
			unsigned int gchanges)
{
	unsigned int changes = dev->flags ^ old_flags;

	if (gchanges)
		rtmsg_ifinfo(RTM_NEWLINK, dev, gchanges, GFP_ATOMIC);

	if (changes & IFF_UP) {
		if (dev->flags & IFF_UP)
			call_netdevice_notifiers(NETDEV_UP, dev);
		else
			call_netdevice_notifiers(NETDEV_DOWN, dev);
	}

	if (dev->flags & IFF_UP &&
	    (changes & ~(IFF_UP | IFF_PROMISC | IFF_ALLMULTI | IFF_VOLATILE))) {
		struct netdev_notifier_change_info change_info = {
			.info = {
				.dev = dev,
			},
			.flags_changed = changes,
		};

		call_netdevice_notifiers_info(NETDEV_CHANGE, &change_info.info);
	}