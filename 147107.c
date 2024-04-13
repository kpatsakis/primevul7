otherinfo(ParsedURL *target, ParsedURL *current, char *referer)
{
    Str s = Strnew();
    const int *no_referer_ptr;
    int no_referer;

    Strcat_charp(s, "User-Agent: ");
    if (UserAgent == NULL || *UserAgent == '\0')
	Strcat_charp(s, w3m_version);
    else
	Strcat_charp(s, UserAgent);
    Strcat_charp(s, "\r\n");

    Strcat_m_charp(s, "Accept: ", AcceptMedia, "\r\n", NULL);
    Strcat_m_charp(s, "Accept-Encoding: ", AcceptEncoding, "\r\n", NULL);
    Strcat_m_charp(s, "Accept-Language: ", AcceptLang, "\r\n", NULL);

    if (target->host) {
	Strcat_charp(s, "Host: ");
	Strcat_charp(s, target->host);
	if (target->port != DefaultPort[target->scheme])
	    Strcat(s, Sprintf(":%d", target->port));
	Strcat_charp(s, "\r\n");
    }
    if (target->is_nocache || NoCache) {
	Strcat_charp(s, "Pragma: no-cache\r\n");
	Strcat_charp(s, "Cache-control: no-cache\r\n");
    }
    no_referer = NoSendReferer;
    no_referer_ptr = query_SCONF_NO_REFERER_FROM(current);
    no_referer = no_referer || (no_referer_ptr && *no_referer_ptr);
    no_referer_ptr = query_SCONF_NO_REFERER_TO(target);
    no_referer = no_referer || (no_referer_ptr && *no_referer_ptr);
    if (!no_referer) {
#ifdef USE_SSL
        if (current && current->scheme == SCM_HTTPS && target->scheme != SCM_HTTPS) {
	  /* Don't send Referer: if https:// -> http:// */
	}
	else
#endif
	if (referer == NULL && current && current->scheme != SCM_LOCAL &&
	    current->scheme != SCM_LOCAL_CGI &&
	    (current->scheme != SCM_FTP ||
	     (current->user == NULL && current->pass == NULL))) {
	    char *p = current->label;
	    Strcat_charp(s, "Referer: ");
	    current->label = NULL;
	    Strcat(s, parsedURL2Str(current));
	    current->label = p;
	    Strcat_charp(s, "\r\n");
	}
	else if (referer != NULL && referer != NO_REFERER) {
	    char *p = strchr(referer, '#');
	    Strcat_charp(s, "Referer: ");
	    if (p)
		Strcat_charp_n(s, referer, p - referer);
	    else
		Strcat_charp(s, referer);
	    Strcat_charp(s, "\r\n");
	}
    }
    return s->ptr;
}