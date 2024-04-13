valid_env_name(const char *name)
{
	const char *cp;

	if (name[0] == '\0')
		return 0;
	for (cp = name; *cp != '\0'; cp++) {
		if (!isalnum((u_char)*cp) && *cp != '_')
			return 0;
	}
	return 1;
}