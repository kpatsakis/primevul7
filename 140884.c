ctlfindtrap(
	sockaddr_u *raddr,
	struct interface *linter
	)
{
	size_t	n;

	for (n = 0; n < COUNTOF(ctl_traps); n++)
		if ((ctl_traps[n].tr_flags & TRAP_INUSE)
		    && ADDR_PORT_EQ(raddr, &ctl_traps[n].tr_addr)
		    && (linter == ctl_traps[n].tr_localaddr))
			return &ctl_traps[n];

	return NULL;
}