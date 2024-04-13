 */
static bool netdev_has_any_lower_dev(struct net_device *dev)
{
	ASSERT_RTNL();

	return !list_empty(&dev->adj_list.lower);