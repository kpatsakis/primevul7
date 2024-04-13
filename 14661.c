opt_match(const char **opts, const char *term)
{
	if (strncasecmp((*opts), term, strlen(term)) == 0 &&
	    (*opts)[strlen(term)] == '=') {
		*opts += strlen(term) + 1;
		return 1;
	}
	return 0;
}