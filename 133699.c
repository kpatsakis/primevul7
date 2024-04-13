static void __dev_close(struct net_device *dev)
{
	LIST_HEAD(single);

	list_add(&dev->close_list, &single);
	__dev_close_many(&single);
	list_del(&single);
}