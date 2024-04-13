ctl_putdblf(
	const char *	tag,
	int		use_f,
	int		precision,
	double		d
	)
{
	char *cp;
	const char *cq;
	char buffer[200];

	cp = buffer;
	cq = tag;
	while (*cq != '\0')
		*cp++ = *cq++;
	*cp++ = '=';
	INSIST((size_t)(cp - buffer) < sizeof(buffer));
	snprintf(cp, sizeof(buffer) - (cp - buffer), use_f ? "%.*f" : "%.*g",
	    precision, d);
	cp += strlen(cp);
	ctl_putdata(buffer, (unsigned)(cp - buffer), 0);
}