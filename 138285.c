_g_line_get_index_from_pattern (const char *line,
				const char *pattern)
{
	int         line_l, pattern_l;
	const char *l;

	line_l = strlen (line);
	pattern_l = strlen (pattern);

	if ((pattern_l == 0) || (line_l == 0))
		return -1;

	for (l = line; *l != 0; l++)
		if (_g_line_matches_pattern (l, pattern))
			return (l - line);

	return -1;
}