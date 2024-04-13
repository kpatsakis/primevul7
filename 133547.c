 */
static int dev_new_index(struct net *net)
{
	int ifindex = net->ifindex;

	for (;;) {
		if (++ifindex <= 0)
			ifindex = 1;
		if (!__dev_get_by_index(net, ifindex))
			return net->ifindex = ifindex;
	}