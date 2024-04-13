
int netdev_bind_sb_channel_queue(struct net_device *dev,
				 struct net_device *sb_dev,
				 u8 tc, u16 count, u16 offset)
{
	/* Make certain the sb_dev and dev are already configured */
	if (sb_dev->num_tc >= 0 || tc >= dev->num_tc)
		return -EINVAL;

	/* We cannot hand out queues we don't have */
	if ((offset + count) > dev->real_num_tx_queues)
		return -EINVAL;

	/* Record the mapping */
	sb_dev->tc_to_txq[tc].count = count;
	sb_dev->tc_to_txq[tc].offset = offset;

	/* Provide a way for Tx queue to find the tc_to_txq map or
	 * XPS map for itself.
	 */
	while (count--)
		netdev_get_tx_queue(dev, count + offset)->sb_dev = sb_dev;

	return 0;