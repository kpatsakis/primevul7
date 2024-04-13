
int netdev_set_sb_channel(struct net_device *dev, u16 channel)
{
	/* Do not use a multiqueue device to represent a subordinate channel */
	if (netif_is_multiqueue(dev))
		return -ENODEV;

	/* We allow channels 1 - 32767 to be used for subordinate channels.
	 * Channel 0 is meant to be "native" mode and used only to represent
	 * the main root device. We allow writing 0 to reset the device back
	 * to normal mode after being used as a subordinate channel.
	 */
	if (channel > S16_MAX)
		return -EINVAL;

	dev->num_tc = -channel;

	return 0;