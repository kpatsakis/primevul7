static const char *diff_funcname_pattern(struct diff_filespec *one)
{
	const char *ident, *pattern;
	int i;

	diff_filespec_check_attr(one);
	ident = one->funcname_pattern_ident;

	if (!ident)
		/*
		 * If the config file has "funcname.default" defined, that
		 * regexp is used; otherwise NULL is returned and xemit uses
		 * the built-in default.
		 */
		return funcname_pattern("default");

	/* Look up custom "funcname.$ident" regexp from config. */
	pattern = funcname_pattern(ident);
	if (pattern)
		return pattern;

	/*
	 * And define built-in fallback patterns here.  Note that
	 * these can be overridden by the user's config settings.
	 */
	for (i = 0; i < ARRAY_SIZE(builtin_funcname_pattern); i++)
		if (!strcmp(ident, builtin_funcname_pattern[i].name))
			return builtin_funcname_pattern[i].pattern;

	return NULL;
}