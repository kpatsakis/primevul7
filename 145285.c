nntohost(
	sockaddr_u *netnum
	)
{
	return nntohost_col(netnum, LIB_BUFLENGTH - 1, FALSE);
}