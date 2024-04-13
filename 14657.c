lookup_env_in_list(const char *env, char * const *envs, size_t nenvs)
{
	size_t i, envlen;

	envlen = strlen(env);
	for (i = 0; i < nenvs; i++) {
		if (strncmp(envs[i], env, envlen) == 0 &&
		    envs[i][envlen] == '=') {
			return envs[i] + envlen + 1;
		}
	}
	return NULL;
}