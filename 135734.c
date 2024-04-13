static void add_header_grep(struct rev_info *revs, const char *field, const char *pattern)
{
	char *pat;
	const char *prefix;
	int patlen, fldlen;

	fldlen = strlen(field);
	patlen = strlen(pattern);
	pat = xmalloc(patlen + fldlen + 10);
	prefix = ".*";
	if (*pattern == '^') {
		prefix = "";
		pattern++;
	}
	sprintf(pat, "^%s %s%s", field, prefix, pattern);
	add_grep(revs, pat, GREP_PATTERN_HEAD);
}