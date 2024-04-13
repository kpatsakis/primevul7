parseURL2(char *url, ParsedURL *pu, ParsedURL *current)
{
    char *p;
    Str tmp;
    int relative_uri = FALSE;

    parseURL(url, pu, current);
#ifndef USE_W3MMAILER
    if (pu->scheme == SCM_MAILTO)
	return;
#endif
    if (pu->scheme == SCM_DATA)
	return;
    if (pu->scheme == SCM_NEWS || pu->scheme == SCM_NEWS_GROUP) {
	if (pu->file && !strchr(pu->file, '@') &&
	    (!(p = strchr(pu->file, '/')) || strchr(p + 1, '-') ||
	     *(p + 1) == '\0'))
	    pu->scheme = SCM_NEWS_GROUP;
	else
	    pu->scheme = SCM_NEWS;
	return;
    }
    if (pu->scheme == SCM_NNTP || pu->scheme == SCM_NNTP_GROUP) {
	if (pu->file && *pu->file == '/')
	    pu->file = allocStr(pu->file + 1, -1);
	if (pu->file && !strchr(pu->file, '@') &&
	    (!(p = strchr(pu->file, '/')) || strchr(p + 1, '-') ||
	     *(p + 1) == '\0'))
	    pu->scheme = SCM_NNTP_GROUP;
	else
	    pu->scheme = SCM_NNTP;
	if (current && (current->scheme == SCM_NNTP ||
			current->scheme == SCM_NNTP_GROUP)) {
	    if (pu->host == NULL) {
		pu->host = current->host;
		pu->port = current->port;
	    }
	}
	return;
    }
    if (pu->scheme == SCM_LOCAL) {
	char *q = expandName(file_unquote(pu->file));
#ifdef SUPPORT_DOS_DRIVE_PREFIX
	Str drive;
	if (IS_ALPHA(q[0]) && q[1] == ':') {
	    drive = Strnew_charp_n(q, 2);
	    Strcat_charp(drive, file_quote(q+2));
	    pu->file = drive->ptr;
	}
	else
#endif
	    pu->file = file_quote(q);
    }

    if (current && (pu->scheme == current->scheme ||
		    (pu->scheme == SCM_FTP && current->scheme == SCM_FTPDIR) ||
		    (pu->scheme == SCM_LOCAL &&
		     current->scheme == SCM_LOCAL_CGI))
	&& pu->host == NULL) {
	/* Copy omitted element from the current URL */
	pu->user = current->user;
	pu->pass = current->pass;
	pu->host = current->host;
	pu->port = current->port;
	if (pu->file && *pu->file) {
#ifdef USE_EXTERNAL_URI_LOADER
	    if (pu->scheme == SCM_UNKNOWN
		&& strchr(pu->file, ':') == NULL
		&& current && (p = strchr(current->file, ':')) != NULL) {
		pu->file = Sprintf("%s:%s",
				   allocStr(current->file,
					    p - current->file), pu->file)->ptr;
	    }
	    else
#endif
		if (
#ifdef USE_GOPHER
		       pu->scheme != SCM_GOPHER &&
#endif				/* USE_GOPHER */
		       pu->file[0] != '/'
#ifdef SUPPORT_DOS_DRIVE_PREFIX
		       && !(pu->scheme == SCM_LOCAL && IS_ALPHA(pu->file[0])
			    && pu->file[1] == ':')
#endif
		) {
		/* file is relative [process 1] */
		p = pu->file;
		if (current->file) {
		    tmp = Strnew_charp(current->file);
		    while (tmp->length > 0) {
			if (Strlastchar(tmp) == '/')
			    break;
			Strshrink(tmp, 1);
		    }
		    Strcat_charp(tmp, p);
		    pu->file = tmp->ptr;
		    relative_uri = TRUE;
		}
	    }
#ifdef USE_GOPHER
	    else if (pu->scheme == SCM_GOPHER && pu->file[0] == '/') {
		p = pu->file;
		pu->file = allocStr(p + 1, -1);
	    }
#endif				/* USE_GOPHER */
	}
	else {			/* scheme:[?query][#label] */
	    pu->file = current->file;
	    if (!pu->query)
		pu->query = current->query;
	}
	/* comment: query part need not to be completed
	 * from the current URL. */
    }
    if (pu->file) {
#ifdef __EMX__
	if (pu->scheme == SCM_LOCAL) {
	    if (strncmp(pu->file, "/$LIB/", 6)) {
		char abs[_MAX_PATH];

		_abspath(abs, file_unquote(pu->file), _MAX_PATH);
		pu->file = file_quote(cleanupName(abs));
	    }
	}
#else
	if (pu->scheme == SCM_LOCAL && pu->file[0] != '/' &&
#ifdef SUPPORT_DOS_DRIVE_PREFIX	/* for 'drive:' */
	    !(IS_ALPHA(pu->file[0]) && pu->file[1] == ':') &&
#endif
	    strcmp(pu->file, "-")) {
	    /* local file, relative path */
	    tmp = Strnew_charp(CurrentDir);
	    if (Strlastchar(tmp) != '/')
		Strcat_char(tmp, '/');
	    Strcat_charp(tmp, file_unquote(pu->file));
	    pu->file = file_quote(cleanupName(tmp->ptr));
	}
#endif
	else if (pu->scheme == SCM_HTTP
#ifdef USE_SSL
		 || pu->scheme == SCM_HTTPS
#endif
	    ) {
	    if (relative_uri) {
		/* In this case, pu->file is created by [process 1] above.
		 * pu->file may contain relative path (for example, 
		 * "/foo/../bar/./baz.html"), cleanupName() must be applied.
		 * When the entire abs_path is given, it still may contain
		 * elements like `//', `..' or `.' in the pu->file. It is 
		 * server's responsibility to canonicalize such path.
		 */
		pu->file = cleanupName(pu->file);
	    }
	}
	else if (
#ifdef USE_GOPHER
		    pu->scheme != SCM_GOPHER &&
#endif				/* USE_GOPHER */
		    pu->file[0] == '/') {
	    /*
	     * this happens on the following conditions:
	     * (1) ftp scheme (2) local, looks like absolute path.
	     * In both case, there must be no side effect with
	     * cleanupName(). (I hope so...)
	     */
	    pu->file = cleanupName(pu->file);
	}
	if (pu->scheme == SCM_LOCAL) {
#ifdef SUPPORT_NETBIOS_SHARE
	    if (pu->host && strcmp(pu->host, "localhost") != 0) {
		Str tmp = Strnew_charp("//");
		Strcat_m_charp(tmp, pu->host,
			       cleanupName(file_unquote(pu->file)), NULL);
		pu->real_file = tmp->ptr;
	    }
	    else
#endif
		pu->real_file = cleanupName(file_unquote(pu->file));
	}
    }
}