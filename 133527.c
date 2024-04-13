
static void init_gro_hash(struct napi_struct *napi)
{
	int i;

	for (i = 0; i < GRO_HASH_BUCKETS; i++) {
		INIT_LIST_HEAD(&napi->gro_hash[i].list);
		napi->gro_hash[i].count = 0;
	}
	napi->gro_bitmask = 0;