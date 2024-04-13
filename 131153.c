int git_config_perm(const char *var, const char *value)
{
	if (value) {
		int i;
		if (!strcmp(value, "umask"))
			return PERM_UMASK;
		if (!strcmp(value, "group"))
			return PERM_GROUP;
		if (!strcmp(value, "all") ||
		    !strcmp(value, "world") ||
		    !strcmp(value, "everybody"))
			return PERM_EVERYBODY;
		i = atoi(value);
		if (i > 1)
			return i;
	}
	return git_config_bool(var, value);
}