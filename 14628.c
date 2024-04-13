opt_dequote(const char **sp, const char **errstrp)
{
	const char *s = *sp;
	char *ret;
	size_t i;

	*errstrp = NULL;
	if (*s != '"') {
		*errstrp = "missing start quote";
		return NULL;
	}
	s++;
	if ((ret = malloc(strlen((s)) + 1)) == NULL) {
		*errstrp = "memory allocation failed";
		return NULL;
	}
	for (i = 0; *s != '\0' && *s != '"';) {
		if (s[0] == '\\' && s[1] == '"')
			s++;
		ret[i++] = *s++;
	}
	if (*s == '\0') {
		*errstrp = "missing end quote";
		free(ret);
		return NULL;
	}
	ret[i] = '\0';
	s++;
	*sp = s;
	return ret;
}