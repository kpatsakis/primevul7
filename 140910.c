ctl_putadr(
	const char *tag,
	u_int32 addr32,
	sockaddr_u *addr
	)
{
	register char *cp;
	register const char *cq;
	char buffer[200];

	cp = buffer;
	cq = tag;
	while (*cq != '\0')
		*cp++ = *cq++;

	*cp++ = '=';
	if (NULL == addr)
		cq = numtoa(addr32);
	else
		cq = stoa(addr);
	INSIST((cp - buffer) < (int)sizeof(buffer));
	snprintf(cp, sizeof(buffer) - (cp - buffer), "%s", cq);
	cp += strlen(cp);
	ctl_putdata(buffer, (unsigned)(cp - buffer), 0);
}