read_addr_restrictions(
	struct recvbuf *	rbufp
)
{
	u_int idx;

	idx = 0;
	send_restrict_list(restrictlist4, FALSE, &idx);
	send_restrict_list(restrictlist6, TRUE, &idx);
	ctl_flushpkt(0);
}