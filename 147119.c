check_no_proxy(char *domain)
{
    TextListItem *tl;
    volatile int ret = 0;
    MySignalHandler(*volatile prevtrap) (SIGNAL_ARG) = NULL;

    if (NO_proxy_domains == NULL || NO_proxy_domains->nitem == 0 ||
	domain == NULL)
	return 0;
    for (tl = NO_proxy_domains->first; tl != NULL; tl = tl->next) {
	if (domain_match(tl->ptr, domain))
	    return 1;
    }
    if (!NOproxy_netaddr) {
	return 0;
    }
    /* 
     * to check noproxy by network addr
     */
    if (SETJMP(AbortLoading) != 0) {
	ret = 0;
	goto end;
    }
    TRAP_ON;
    {
#ifndef INET6
	struct hostent *he;
	int n;
	unsigned char **h_addr_list;
	char addr[4 * 16], buf[5];

	he = gethostbyname(domain);
	if (!he) {
	    ret = 0;
	    goto end;
	}
	for (h_addr_list = (unsigned char **)he->h_addr_list; *h_addr_list;
	     h_addr_list++) {
	    sprintf(addr, "%d", h_addr_list[0][0]);
	    for (n = 1; n < he->h_length; n++) {
		sprintf(buf, ".%d", h_addr_list[0][n]);
		strcat(addr, buf);
	    }
	    for (tl = NO_proxy_domains->first; tl != NULL; tl = tl->next) {
		if (strncmp(tl->ptr, addr, strlen(tl->ptr)) == 0) {
		    ret = 1;
		    goto end;
		}
	    }
	}
#else				/* INET6 */
	int error;
	struct addrinfo hints;
	struct addrinfo *res, *res0;
	char addr[4 * 16];
	int *af;

	for (af = ai_family_order_table[DNS_order];; af++) {
	    memset(&hints, 0, sizeof(hints));
	    hints.ai_family = *af;
	    error = getaddrinfo(domain, NULL, &hints, &res0);
	    if (error) {
		if (*af == PF_UNSPEC) {
		    break;
		}
		/* try next */
		continue;
	    }
	    for (res = res0; res != NULL; res = res->ai_next) {
		switch (res->ai_family) {
		case AF_INET:
		    inet_ntop(AF_INET,
			      &((struct sockaddr_in *)res->ai_addr)->sin_addr,
			      addr, sizeof(addr));
		    break;
		case AF_INET6:
		    inet_ntop(AF_INET6,
			      &((struct sockaddr_in6 *)res->ai_addr)->
			      sin6_addr, addr, sizeof(addr));
		    break;
		default:
		    /* unknown */
		    continue;
		}
		for (tl = NO_proxy_domains->first; tl != NULL; tl = tl->next) {
		    if (strncmp(tl->ptr, addr, strlen(tl->ptr)) == 0) {
			freeaddrinfo(res0);
			ret = 1;
			goto end;
		    }
		}
	    }
	    freeaddrinfo(res0);
	    if (*af == PF_UNSPEC) {
		break;
	    }
	}
#endif				/* INET6 */
    }
  end:
    TRAP_OFF;
    return ret;
}