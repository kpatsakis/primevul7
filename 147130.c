getURLScheme(char **url)
{
    char *p = *url, *q;
    int i;
    int scheme = SCM_MISSING;

    while (*p && (IS_ALNUM(*p) || *p == '.' || *p == '+' || *p == '-'))
	p++;
    if (*p == ':') {		/* scheme found */
	scheme = SCM_UNKNOWN;
	for (i = 0; (q = schemetable[i].cmdname) != NULL; i++) {
	    int len = strlen(q);
	    if (!strncasecmp(q, *url, len) && (*url)[len] == ':') {
		scheme = schemetable[i].cmd;
		*url = p + 1;
		break;
	    }
	}
    }
    return scheme;
}