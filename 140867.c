ctl_putarray(
	const char *tag,
	double *arr,
	int start
	)
{
	register char *cp;
	register const char *cq;
	char buffer[200];
	int i;
	cp = buffer;
	cq = tag;
	while (*cq != '\0')
		*cp++ = *cq++;
	*cp++ = '=';
	i = start;
	do {
		if (i == 0)
			i = NTP_SHIFT;
		i--;
		INSIST((cp - buffer) < (int)sizeof(buffer));
		snprintf(cp, sizeof(buffer) - (cp - buffer),
			 " %.2f", arr[i] * 1e3);
		cp += strlen(cp);
	} while (i != start);
	ctl_putdata(buffer, (unsigned)(cp - buffer), 0);
}