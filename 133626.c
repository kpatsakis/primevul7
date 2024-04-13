
static void __net_exit netdev_exit(struct net *net)
{
	kfree(net->dev_name_head);
	kfree(net->dev_index_head);
	if (net != &init_net)
		WARN_ON_ONCE(!list_empty(&net->dev_base_head));