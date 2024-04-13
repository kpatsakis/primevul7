ipsecmod_apply_cfg(struct ipsecmod_env* ipsecmod_env, struct config_file* cfg)
{
	if(!cfg->ipsecmod_hook || (cfg->ipsecmod_hook && !cfg->ipsecmod_hook[0])) {
		log_err("ipsecmod: missing ipsecmod-hook.");
		return 0;
	}
	if(cfg->ipsecmod_whitelist &&
		!ipsecmod_whitelist_apply_cfg(ipsecmod_env, cfg))
		return 0;
	return 1;
}