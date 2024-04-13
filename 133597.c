 */
bool napi_hash_del(struct napi_struct *napi)
{
	bool rcu_sync_needed = false;

	spin_lock(&napi_hash_lock);

	if (test_and_clear_bit(NAPI_STATE_HASHED, &napi->state)) {
		rcu_sync_needed = true;
		hlist_del_rcu(&napi->napi_hash_node);
	}
	spin_unlock(&napi_hash_lock);
	return rcu_sync_needed;