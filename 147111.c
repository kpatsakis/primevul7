HTTPrequest(ParsedURL *pu, ParsedURL *current, HRequest *hr, TextList *extra)
{
    Str tmp;
    TextListItem *i;
#ifdef USE_COOKIE
    Str cookie;
#endif				/* USE_COOKIE */
    tmp = HTTPrequestMethod(hr);
    Strcat_charp(tmp, " ");
    Strcat_charp(tmp, HTTPrequestURI(pu, hr)->ptr);
    Strcat_charp(tmp, " HTTP/1.0\r\n");
    if (hr->referer == NO_REFERER)
	Strcat_charp(tmp, otherinfo(pu, NULL, NULL));
    else
	Strcat_charp(tmp, otherinfo(pu, current, hr->referer));
    if (extra != NULL)
	for (i = extra->first; i != NULL; i = i->next) {
	    if (strncasecmp(i->ptr, "Authorization:",
			    sizeof("Authorization:") - 1) == 0) {
#ifdef USE_SSL
		if (hr->command == HR_COMMAND_CONNECT)
		    continue;
#endif
	    }
	    if (strncasecmp(i->ptr, "Proxy-Authorization:",
			    sizeof("Proxy-Authorization:") - 1) == 0) {
#ifdef USE_SSL
		if (pu->scheme == SCM_HTTPS
		    && hr->command != HR_COMMAND_CONNECT)
		    continue;
#endif
	    }
	    Strcat_charp(tmp, i->ptr);
	}

#ifdef USE_COOKIE
    if (hr->command != HR_COMMAND_CONNECT &&
	use_cookie && (cookie = find_cookie(pu))) {
	Strcat_charp(tmp, "Cookie: ");
	Strcat(tmp, cookie);
	Strcat_charp(tmp, "\r\n");
	/* [DRAFT 12] s. 10.1 */
	if (cookie->ptr[0] != '$')
	    Strcat_charp(tmp, "Cookie2: $Version=\"1\"\r\n");
    }
#endif				/* USE_COOKIE */
    if (hr->command == HR_COMMAND_POST) {
	if (hr->request->enctype == FORM_ENCTYPE_MULTIPART) {
	    Strcat_charp(tmp, "Content-Type: multipart/form-data; boundary=");
	    Strcat_charp(tmp, hr->request->boundary);
	    Strcat_charp(tmp, "\r\n");
	    Strcat(tmp,
		   Sprintf("Content-Length: %ld\r\n", hr->request->length));
	    Strcat_charp(tmp, "\r\n");
	}
	else {
	    if (!override_content_type) {
		Strcat_charp(tmp,
			     "Content-Type: application/x-www-form-urlencoded\r\n");
	    }
	    Strcat(tmp,
		   Sprintf("Content-Length: %ld\r\n", hr->request->length));
	    if (header_string)
		Strcat(tmp, header_string);
	    Strcat_charp(tmp, "\r\n");
	    Strcat_charp_n(tmp, hr->request->body, hr->request->length);
	    Strcat_charp(tmp, "\r\n");
	}
    }
    else {
	if (header_string)
	    Strcat(tmp, header_string);
	Strcat_charp(tmp, "\r\n");
    }
#ifdef DEBUG
    fprintf(stderr, "HTTPrequest: [ %s ]\n\n", tmp->ptr);
#endif				/* DEBUG */
    return tmp;
}