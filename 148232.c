ipsecmod_deinit(struct module_env* env, int id)
{
	struct ipsecmod_env* ipsecmod_env;
	if(!env || !env->modinfo[id])
		return;
	ipsecmod_env = (struct ipsecmod_env*)env->modinfo[id];
	/* Free contents. */
	ipsecmod_whitelist_delete(ipsecmod_env->whitelist);
	free(ipsecmod_env);
	env->modinfo[id] = NULL;
}