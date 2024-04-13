ipsecmod_init(struct module_env* env, int id)
{
	struct ipsecmod_env* ipsecmod_env = (struct ipsecmod_env*)calloc(1,
		sizeof(struct ipsecmod_env));
	if(!ipsecmod_env) {
		log_err("malloc failure");
		return 0;
	}
	env->modinfo[id] = (void*)ipsecmod_env;
	ipsecmod_env->whitelist = NULL;
	if(!ipsecmod_apply_cfg(ipsecmod_env, env->cfg)) {
		log_err("ipsecmod: could not apply configuration settings.");
		return 0;
	}
	return 1;
}