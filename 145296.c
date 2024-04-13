nntohost_col(
	sockaddr_u *	addr,
	size_t		width,
	int		preserve_lowaddrbits
	)
{
	const char *	out;

	if (!showhostnames || SOCK_UNSPEC(addr)) {
		if (preserve_lowaddrbits)
			out = trunc_left(stoa(addr), width);
		else
			out = trunc_right(stoa(addr), width);
	} else if (ISREFCLOCKADR(addr)) {
		out = refnumtoa(addr);
	} else {
		out = trunc_right(socktohost(addr), width);
	}
	return out;
}