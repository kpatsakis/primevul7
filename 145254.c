nntohostp(
	sockaddr_u *netnum
	)
{
	const char *	hostn;
	char *		buf;

	if (!showhostnames || SOCK_UNSPEC(netnum))
		return sptoa(netnum);
	else if (ISREFCLOCKADR(netnum))
		return refnumtoa(netnum);

	hostn = socktohost(netnum);
	LIB_GETBUF(buf);
	snprintf(buf, LIB_BUFLENGTH, "%s:%u", hostn, SRCPORT(netnum));

	return buf;
}