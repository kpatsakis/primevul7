openURL(char *url, ParsedURL *pu, ParsedURL *current,
	URLOption *option, FormList *request, TextList *extra_header,
	URLFile *ouf, HRequest *hr, unsigned char *status)
{
    Str tmp;
    int sock, scheme;
    char *p, *q, *u;
    URLFile uf;
    HRequest hr0;
#ifdef USE_SSL
    SSL *sslh = NULL;
#endif				/* USE_SSL */

    if (hr == NULL)
	hr = &hr0;

    if (ouf) {
	uf = *ouf;
    }
    else {
	init_stream(&uf, SCM_MISSING, NULL);
    }

    u = url;
    scheme = getURLScheme(&u);
    if (current == NULL && scheme == SCM_MISSING && !ArgvIsURL)
	u = file_to_url(url);	/* force to local file */
    else
	u = url;
  retry:
    parseURL2(u, pu, current);
    if (pu->scheme == SCM_LOCAL && pu->file == NULL) {
	if (pu->label != NULL) {
	    /* #hogege is not a label but a filename */
	    Str tmp2 = Strnew_charp("#");
	    Strcat_charp(tmp2, pu->label);
	    pu->file = tmp2->ptr;
	    pu->real_file = cleanupName(file_unquote(pu->file));
	    pu->label = NULL;
	}
	else {
	    /* given URL must be null string */
#ifdef SOCK_DEBUG
	    sock_log("given URL must be null string\n");
#endif
	    return uf;
	}
    }

    uf.scheme = pu->scheme;
    uf.url = parsedURL2Str(pu)->ptr;
    pu->is_nocache = (option->flag & RG_NOCACHE);
    uf.ext = filename_extension(pu->file, 1);

    hr->command = HR_COMMAND_GET;
    hr->flag = 0;
    hr->referer = option->referer;
    hr->request = request;

    switch (pu->scheme) {
    case SCM_LOCAL:
    case SCM_LOCAL_CGI:
	if (request && request->body)
	    /* local CGI: POST */
	    uf.stream = newFileStream(localcgi_post(pu->real_file, pu->query,
						    request, option->referer),
				      (void (*)())fclose);
	else
	    /* lodal CGI: GET */
	    uf.stream = newFileStream(localcgi_get(pu->real_file, pu->query,
						   option->referer),
				      (void (*)())fclose);
	if (uf.stream) {
	    uf.is_cgi = TRUE;
	    uf.scheme = pu->scheme = SCM_LOCAL_CGI;
	    return uf;
	}
	examineFile(pu->real_file, &uf);
	if (uf.stream == NULL) {
	    if (dir_exist(pu->real_file)) {
		add_index_file(pu, &uf);
		if (uf.stream == NULL)
		    return uf;
	    }
	    else if (document_root != NULL) {
		tmp = Strnew_charp(document_root);
		if (Strlastchar(tmp) != '/' && pu->file[0] != '/')
		    Strcat_char(tmp, '/');
		Strcat_charp(tmp, pu->file);
		p = cleanupName(tmp->ptr);
		q = cleanupName(file_unquote(p));
		if (dir_exist(q)) {
		    pu->file = p;
		    pu->real_file = q;
		    add_index_file(pu, &uf);
		    if (uf.stream == NULL) {
			return uf;
		    }
		}
		else {
		    examineFile(q, &uf);
		    if (uf.stream) {
			pu->file = p;
			pu->real_file = q;
		    }
		}
	    }
	}
	if (uf.stream == NULL && retryAsHttp && url[0] != '/') {
	    if (scheme == SCM_MISSING || scheme == SCM_UNKNOWN) {
		/* retry it as "http://" */
		u = Strnew_m_charp("http://", url, NULL)->ptr;
		goto retry;
	    }
	}
	return uf;
    case SCM_FTP:
    case SCM_FTPDIR:
	if (pu->file == NULL)
	    pu->file = allocStr("/", -1);
	if (non_null(FTP_proxy) &&
	    !Do_not_use_proxy &&
	    pu->host != NULL && !check_no_proxy(pu->host)) {
	    hr->flag |= HR_FLAG_PROXY;
	    sock = openSocket(FTP_proxy_parsed.host,
			      schemeNumToName(FTP_proxy_parsed.scheme),
			      FTP_proxy_parsed.port);
	    if (sock < 0)
		return uf;
	    uf.scheme = SCM_HTTP;
	    tmp = HTTPrequest(pu, current, hr, extra_header);
	    write(sock, tmp->ptr, tmp->length);
	}
	else {
	    uf.stream = openFTPStream(pu, &uf);
	    uf.scheme = pu->scheme;
	    return uf;
	}
	break;
    case SCM_HTTP:
#ifdef USE_SSL
    case SCM_HTTPS:
#endif				/* USE_SSL */
	if (pu->file == NULL)
	    pu->file = allocStr("/", -1);
	if (request && request->method == FORM_METHOD_POST && request->body)
	    hr->command = HR_COMMAND_POST;
	if (request && request->method == FORM_METHOD_HEAD)
	    hr->command = HR_COMMAND_HEAD;
	if ((
#ifdef USE_SSL
		(pu->scheme == SCM_HTTPS) ? non_null(HTTPS_proxy) :
#endif				/* USE_SSL */
		non_null(HTTP_proxy)) && !Do_not_use_proxy &&
	    pu->host != NULL && !check_no_proxy(pu->host)) {
	    hr->flag |= HR_FLAG_PROXY;
#ifdef USE_SSL
	    if (pu->scheme == SCM_HTTPS && *status == HTST_CONNECT) {
		sock = ssl_socket_of(ouf->stream);
		if (!(sslh = openSSLHandle(sock, pu->host,
					   &uf.ssl_certificate))) {
		    *status = HTST_MISSING;
		    return uf;
		}
	    }
	    else if (pu->scheme == SCM_HTTPS) {
		sock = openSocket(HTTPS_proxy_parsed.host,
				  schemeNumToName(HTTPS_proxy_parsed.scheme),
				  HTTPS_proxy_parsed.port);
		sslh = NULL;
	    }
	    else {
#endif				/* USE_SSL */
		sock = openSocket(HTTP_proxy_parsed.host,
				  schemeNumToName(HTTP_proxy_parsed.scheme),
				  HTTP_proxy_parsed.port);
#ifdef USE_SSL
		sslh = NULL;
	    }
#endif				/* USE_SSL */
	    if (sock < 0) {
#ifdef SOCK_DEBUG
		sock_log("Can't open socket\n");
#endif
		return uf;
	    }
#ifdef USE_SSL
	    if (pu->scheme == SCM_HTTPS) {
		if (*status == HTST_NORMAL) {
		    hr->command = HR_COMMAND_CONNECT;
		    tmp = HTTPrequest(pu, current, hr, extra_header);
		    *status = HTST_CONNECT;
		}
		else {
		    hr->flag |= HR_FLAG_LOCAL;
		    tmp = HTTPrequest(pu, current, hr, extra_header);
		    *status = HTST_NORMAL;
		}
	    }
	    else
#endif				/* USE_SSL */
	    {
		tmp = HTTPrequest(pu, current, hr, extra_header);
		*status = HTST_NORMAL;
	    }
	}
	else {
	    sock = openSocket(pu->host, schemeNumToName(pu->scheme), pu->port);
	    if (sock < 0) {
		*status = HTST_MISSING;
		return uf;
	    }
#ifdef USE_SSL
	    if (pu->scheme == SCM_HTTPS) {
		if (!(sslh = openSSLHandle(sock, pu->host,
					   &uf.ssl_certificate))) {
		    *status = HTST_MISSING;
		    return uf;
		}
	    }
#endif				/* USE_SSL */
	    hr->flag |= HR_FLAG_LOCAL;
	    tmp = HTTPrequest(pu, current, hr, extra_header);
	    *status = HTST_NORMAL;
	}
#ifdef USE_SSL
	if (pu->scheme == SCM_HTTPS) {
	    uf.stream = newSSLStream(sslh, sock);
	    if (sslh)
		SSL_write(sslh, tmp->ptr, tmp->length);
	    else
		write(sock, tmp->ptr, tmp->length);
	    if(w3m_reqlog){
		FILE *ff = fopen(w3m_reqlog, "a");
		if (sslh)
		    fputs("HTTPS: request via SSL\n", ff);
		else
		    fputs("HTTPS: request without SSL\n", ff);
		fwrite(tmp->ptr, sizeof(char), tmp->length, ff);
		fclose(ff);
	    }
	    if (hr->command == HR_COMMAND_POST &&
		request->enctype == FORM_ENCTYPE_MULTIPART) {
		if (sslh)
		    SSL_write_from_file(sslh, request->body);
		else
		    write_from_file(sock, request->body);
	    }
	    return uf;
	}
	else
#endif				/* USE_SSL */
	{
	    write(sock, tmp->ptr, tmp->length);
	    if(w3m_reqlog){
		FILE *ff = fopen(w3m_reqlog, "a");
		fwrite(tmp->ptr, sizeof(char), tmp->length, ff);
		fclose(ff);
	    }
	    if (hr->command == HR_COMMAND_POST &&
		request->enctype == FORM_ENCTYPE_MULTIPART)
		write_from_file(sock, request->body);
	}
	break;
#ifdef USE_GOPHER
    case SCM_GOPHER:
	if (non_null(GOPHER_proxy) &&
	    !Do_not_use_proxy &&
	    pu->host != NULL && !check_no_proxy(pu->host)) {
	    hr->flag |= HR_FLAG_PROXY;
	    sock = openSocket(GOPHER_proxy_parsed.host,
			      schemeNumToName(GOPHER_proxy_parsed.scheme),
			      GOPHER_proxy_parsed.port);
	    if (sock < 0)
		return uf;
	    uf.scheme = SCM_HTTP;
	    tmp = HTTPrequest(pu, current, hr, extra_header);
	}
	else {
	    sock = openSocket(pu->host, schemeNumToName(pu->scheme), pu->port);
	    if (sock < 0)
		return uf;
	    if (pu->file == NULL)
		pu->file = "1";
	    tmp = Strnew_charp(file_unquote(pu->file));
	    Strcat_char(tmp, '\n');
	}
	write(sock, tmp->ptr, tmp->length);
	break;
#endif				/* USE_GOPHER */
#ifdef USE_NNTP
    case SCM_NNTP:
    case SCM_NNTP_GROUP:
    case SCM_NEWS:
    case SCM_NEWS_GROUP:
	if (pu->scheme == SCM_NNTP || pu->scheme == SCM_NEWS)
	    uf.scheme = SCM_NEWS;
	else
	    uf.scheme = SCM_NEWS_GROUP;
	uf.stream = openNewsStream(pu);
	return uf;
#endif				/* USE_NNTP */
    case SCM_DATA:
	if (pu->file == NULL)
	    return uf;
	p = Strnew_charp(pu->file)->ptr;
	q = strchr(p, ',');
	if (q == NULL)
	    return uf;
	*q++ = '\0';
	tmp = Strnew_charp(q);
	q = strrchr(p, ';');
	if (q != NULL && !strcmp(q, ";base64")) {
	    *q = '\0';
	    uf.encoding = ENC_BASE64;
	}
	else
	    tmp = Str_url_unquote(tmp, FALSE, FALSE);
	uf.stream = newStrStream(tmp);
	uf.guess_type = (*p != '\0') ? p : "text/plain";
	return uf;
    case SCM_UNKNOWN:
    default:
	return uf;
    }
    uf.stream = newInputStream(sock);
    return uf;
}