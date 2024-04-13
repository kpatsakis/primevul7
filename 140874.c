ctl_putunqstr(
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
		INSIST(tl + 1 + len <= sizeof(buffer));
		*cp++ = '=';
		memcpy(cp, data, len);
		cp += len;
	}
	ctl_putdata(buffer, (u_int)(cp - buffer), 0);
}