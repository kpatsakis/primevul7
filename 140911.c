sockaddrs_from_restrict_u(
	sockaddr_u *	psaA,
	sockaddr_u *	psaM,
	restrict_u *	pres,
	int		ipv6
	)
{
	ZERO(*psaA);
	ZERO(*psaM);
	if (!ipv6) {
		psaA->sa.sa_family = AF_INET;
		psaA->sa4.sin_addr.s_addr = htonl(pres->u.v4.addr);
		psaM->sa.sa_family = AF_INET;
		psaM->sa4.sin_addr.s_addr = htonl(pres->u.v4.mask);
	} else {
		psaA->sa.sa_family = AF_INET6;
		memcpy(&psaA->sa6.sin6_addr, &pres->u.v6.addr,
		       sizeof(psaA->sa6.sin6_addr));
		psaM->sa.sa_family = AF_INET6;
		memcpy(&psaM->sa6.sin6_addr, &pres->u.v6.mask,
		       sizeof(psaA->sa6.sin6_addr));
	}
}