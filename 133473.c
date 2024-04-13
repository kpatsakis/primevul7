 */
int dev_change_proto_down_generic(struct net_device *dev, bool proto_down)
{
	if (proto_down)
		netif_carrier_off(dev);
	else
		netif_carrier_on(dev);
	dev->proto_down = proto_down;
	return 0;