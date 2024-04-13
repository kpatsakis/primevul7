ctl_putstr(
	const char *	tag,
	const char *	data,
	size_t		len
	)
{
	char buffer[512];
	char *cp;
	size_t tl;

	tl = strlen(tag);
	memcpy(buffer, tag, tl);
	cp = buffer + tl;
	if (len > 0) {
		INSIST(tl + 3 + len <= sizeof(buffer));
		*cp++ = '=';
		*cp++ = '"';
		memcpy(cp, data, len);
		cp += len;
		*cp++ = '"';
	}
	ctl_putdata(buffer, (u_int)(cp - buffer), 0);
}