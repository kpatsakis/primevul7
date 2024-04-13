_g_regexp_split_from_patterns (const char         *pattern_string,
			       GRegexCompileFlags  compile_options)
{
	char   **patterns;
	GRegex **regexps;
	int      i;

	patterns = _g_regexp_get_patternv (pattern_string);
	if (patterns == NULL)
		return NULL;

	regexps = g_new0 (GRegex*, g_strv_length (patterns) + 1);
	for (i = 0; patterns[i] != NULL; i++)
		regexps[i] = g_regex_new (patterns[i],
					  G_REGEX_OPTIMIZE | compile_options,
					  G_REGEX_MATCH_NOTEMPTY,
					  NULL);
	g_strfreev (patterns);

	return regexps;
}