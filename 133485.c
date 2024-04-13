/* Must be called in process context */
void netif_napi_del(struct napi_struct *napi)
{
	might_sleep();
	if (napi_hash_del(napi))
		synchronize_net();
	list_del_init(&napi->dev_list);
	napi_free_frags(napi);

	flush_gro_hash(napi);
	napi->gro_bitmask = 0;