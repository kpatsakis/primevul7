
static void flush_gro_hash(struct napi_struct *napi)
{
	int i;

	for (i = 0; i < GRO_HASH_BUCKETS; i++) {
		struct sk_buff *skb, *n;

		list_for_each_entry_safe(skb, n, &napi->gro_hash[i].list, list)
			kfree_skb(skb);
		napi->gro_hash[i].count = 0;
	}