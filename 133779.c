 */
bool dev_nit_active(struct net_device *dev)
{
	return !list_empty(&ptype_all) || !list_empty(&dev->ptype_all);