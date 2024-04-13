static const char *funcname_pattern(const char *ident)
{
	struct funcname_pattern *pp;

	for (pp = funcname_pattern_list; pp; pp = pp->next)
		if (!strcmp(ident, pp->name))
			return pp->pattern;
	return NULL;
}