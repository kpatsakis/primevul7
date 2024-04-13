struct net_device *dev_get_by_napi_id(unsigned int napi_id)
{
	struct napi_struct *napi;

	WARN_ON_ONCE(!rcu_read_lock_held());

	if (napi_id < MIN_NAPI_ID)
		return NULL;

	napi = napi_by_id(napi_id);

	return napi ? napi->dev : NULL;
}