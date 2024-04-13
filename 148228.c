ipsecmod_get_mem(struct module_env* env, int id)
{
	struct ipsecmod_env* ie = (struct ipsecmod_env*)env->modinfo[id];
	if(!ie)
		return 0;
	return sizeof(*ie) + ipsecmod_whitelist_get_mem(ie->whitelist);
}