getnetnum(
	const char *hname,
	sockaddr_u *num,
	char *fullhost,
	int af
	)
{
	struct addrinfo hints, *ai = NULL;

	ZERO(hints);
	hints.ai_flags = AI_CANONNAME;
#ifdef AI_ADDRCONFIG
	hints.ai_flags |= AI_ADDRCONFIG;
#endif

	/*
	 * decodenetnum only works with addresses, but handles syntax
	 * that getaddrinfo doesn't:  [2001::1]:1234
	 */
	if (decodenetnum(hname, num)) {
		if (fullhost != NULL)
			getnameinfo(&num->sa, SOCKLEN(num), fullhost,
				    LENHOSTNAME, NULL, 0, 0);
		return 1;
	} else if (getaddrinfo(hname, "ntp", &hints, &ai) == 0) {
		INSIST(sizeof(*num) >= ai->ai_addrlen);
		memcpy(num, ai->ai_addr, ai->ai_addrlen);
		if (fullhost != NULL) {
			if (ai->ai_canonname != NULL)
				strlcpy(fullhost, ai->ai_canonname,
					LENHOSTNAME);
			else
				getnameinfo(&num->sa, SOCKLEN(num),
					    fullhost, LENHOSTNAME, NULL,
					    0, 0);
		}
		freeaddrinfo(ai);
		return 1;
	}
	fprintf(stderr, "***Can't find host %s\n", hname);

	return 0;
}