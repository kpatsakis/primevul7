ctl_putfs(
	const char *tag,
	tstamp_t uval
	)
{
	register char *cp;
	register const char *cq;
	char buffer[200];
	struct tm *tm = NULL;
	time_t fstamp;

	cp = buffer;
	cq = tag;
	while (*cq != '\0')
		*cp++ = *cq++;

	*cp++ = '=';
	fstamp = uval - JAN_1970;
	tm = gmtime(&fstamp);
	if (NULL ==  tm)
		return;
	INSIST((cp - buffer) < (int)sizeof(buffer));
	snprintf(cp, sizeof(buffer) - (cp - buffer),
		 "%04d%02d%02d%02d%02d", tm->tm_year + 1900,
		 tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);
	cp += strlen(cp);
	ctl_putdata(buffer, (unsigned)( cp - buffer ), 0);
}