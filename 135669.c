static int parse_funcname_pattern(const char *var, const char *ep, const char *value)
{
	const char *name;
	int namelen;
	struct funcname_pattern *pp;

	name = var + 5; /* "diff." */
	namelen = ep - name;

	for (pp = funcname_pattern_list; pp; pp = pp->next)
		if (!strncmp(pp->name, name, namelen) && !pp->name[namelen])
			break;
	if (!pp) {
		pp = xcalloc(1, sizeof(*pp));
		pp->name = xmemdupz(name, namelen);
		pp->next = funcname_pattern_list;
		funcname_pattern_list = pp;
	}
	free(pp->pattern);
	pp->pattern = xstrdup(value);
	return 0;
}