/* Initialize per network namespace state */
static int __net_init netdev_init(struct net *net)
{
	BUILD_BUG_ON(GRO_HASH_BUCKETS >
		     8 * FIELD_SIZEOF(struct napi_struct, gro_bitmask));

	if (net != &init_net)
		INIT_LIST_HEAD(&net->dev_base_head);

	net->dev_name_head = netdev_create_hash();
	if (net->dev_name_head == NULL)
		goto err_name;

	net->dev_index_head = netdev_create_hash();
	if (net->dev_index_head == NULL)
		goto err_idx;

	return 0;

err_idx:
	kfree(net->dev_name_head);
err_name:
	return -ENOMEM;