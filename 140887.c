ctl_putint(
	const char *tag,
	long ival
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
	INSIST((cp - buffer) < (int)sizeof(buffer));
	snprintf(cp, sizeof(buffer) - (cp - buffer), "%ld", ival);
	cp += strlen(cp);
	ctl_putdata(buffer, (unsigned)( cp - buffer ), 0);
}