send_restrict_list(
	restrict_u *	pres,
	int		ipv6,
	u_int *		pidx
	)
{
	for ( ; pres != NULL; pres = pres->link) {
		send_restrict_entry(pres, ipv6, *pidx);
		(*pidx)++;
	}
}