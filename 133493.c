 */
bool netdev_port_same_parent_id(struct net_device *a, struct net_device *b)
{
	struct netdev_phys_item_id a_id = { };
	struct netdev_phys_item_id b_id = { };

	if (dev_get_port_parent_id(a, &a_id, true) ||
	    dev_get_port_parent_id(b, &b_id, true))
		return false;

	return netdev_phys_item_id_same(&a_id, &b_id);