ctl_putts(
	const char *tag,
	l_fp *ts
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
	INSIST((size_t)(cp - buffer) < sizeof(buffer));
	snprintf(cp, sizeof(buffer) - (cp - buffer), "0x%08x.%08x",
		 (u_int)ts->l_ui, (u_int)ts->l_uf);
	cp += strlen(cp);
	ctl_putdata(buffer, (unsigned)( cp - buffer ), 0);
}