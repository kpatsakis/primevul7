a2port(const char *s)
{
	struct servent *se;
	long long port;
	const char *errstr;

	port = strtonum(s, 0, 65535, &errstr);
	if (errstr == NULL)
		return (int)port;
	if ((se = getservbyname(s, "tcp")) != NULL)
		return ntohs(se->s_port);
	return -1;
}