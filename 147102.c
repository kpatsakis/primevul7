DefaultFile(int scheme)
{
    switch (scheme) {
    case SCM_HTTP:
#ifdef USE_SSL
    case SCM_HTTPS:
#endif				/* USE_SSL */
	return allocStr(HTTP_DEFAULT_FILE, -1);
#ifdef USE_GOPHER
    case SCM_GOPHER:
	return allocStr("1", -1);
#endif				/* USE_GOPHER */
    case SCM_LOCAL:
    case SCM_LOCAL_CGI:
    case SCM_FTP:
    case SCM_FTPDIR:
	return allocStr("/", -1);
    }
    return NULL;
}