searchURIMethods(ParsedURL *pu)
{
    struct table2 *ump;
    int i;
    Str scheme = NULL;
    Str url;
    char *p;

    if (pu->scheme != SCM_UNKNOWN)
	return NULL;		/* use internal */
    if (urimethods == NULL)
	return NULL;
    url = parsedURL2Str(pu);
    for (p = url->ptr; *p != '\0'; p++) {
	if (*p == ':') {
	    scheme = Strnew_charp_n(url->ptr, p - url->ptr);
	    break;
	}
    }
    if (scheme == NULL)
	return NULL;

    /*
     * RFC2396 3.1. Scheme Component
     * For resiliency, programs interpreting URI should treat upper case
     * letters as equivalent to lower case in scheme names (e.g., allow
     * "HTTP" as well as "http").
     */
    for (i = 0; (ump = urimethods[i]) != NULL; i++) {
	for (; ump->item1 != NULL; ump++) {
	    if (strcasecmp(ump->item1, scheme->ptr) == 0) {
		return Sprintf(ump->item2, url_quote(url->ptr));
	    }
	}
    }
    for (ump = default_urimethods; ump->item1 != NULL; ump++) {
	if (strcasecmp(ump->item1, scheme->ptr) == 0) {
	    return Sprintf(ump->item2, url_quote(url->ptr));
	}
    }
    return NULL;
}