
static void napi_hash_add(struct napi_struct *napi)
{
	if (test_bit(NAPI_STATE_NO_BUSY_POLL, &napi->state) ||
	    test_and_set_bit(NAPI_STATE_HASHED, &napi->state))
		return;

	spin_lock(&napi_hash_lock);

	/* 0..NR_CPUS range is reserved for sender_cpu use */
	do {
		if (unlikely(++napi_gen_id < MIN_NAPI_ID))
			napi_gen_id = MIN_NAPI_ID;
	} while (napi_by_id(napi_gen_id));
	napi->napi_id = napi_gen_id;

	hlist_add_head_rcu(&napi->napi_hash_node,
			   &napi_hash[napi->napi_id % HASH_SIZE(napi_hash)]);

	spin_unlock(&napi_hash_lock);