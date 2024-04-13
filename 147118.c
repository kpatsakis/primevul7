domain_match(char *pat, char *domain)
{
    if (domain == NULL)
	return 0;
    if (*pat == '.')
	pat++;
    for (;;) {
	if (!strcasecmp(pat, domain))
	    return 1;
	domain = strchr(domain, '.');
	if (domain == NULL)
	    return 0;
	domain++;
    }
}