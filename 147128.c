schemeToProxy(int scheme)
{
    ParsedURL *pu = NULL;	/* for gcc */
    switch (scheme) {
    case SCM_HTTP:
	pu = &HTTP_proxy_parsed;
	break;
#ifdef USE_SSL
    case SCM_HTTPS:
	pu = &HTTPS_proxy_parsed;
	break;
#endif
    case SCM_FTP:
	pu = &FTP_proxy_parsed;
	break;
#ifdef USE_GOPHER
    case SCM_GOPHER:
	pu = &GOPHER_proxy_parsed;
	break;
#endif
#ifdef DEBUG
    default:
	abort();
#endif
    }
    return pu;
}