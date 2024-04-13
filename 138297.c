_g_regexp_get_patternv (const char *pattern_string)
{
	char **patterns;
	int    i;

	if (pattern_string == NULL)
		return NULL;

	patterns = _g_utf8_strsplit (pattern_string, ";", MAX_PATTERNS);
	for (i = 0; patterns[i] != NULL; i++) {
		char *p1, *p2;

		p1 = g_utf8_strstrip (patterns[i]);
		p2 = _g_str_substitute (p1, ".", "\\.");
		patterns[i] = _g_str_substitute (p2, "*", ".*");

		g_free (p2);
		g_free (p1);
	}

	return patterns;
}