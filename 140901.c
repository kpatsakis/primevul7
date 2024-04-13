ctlsettrap(
	sockaddr_u *raddr,
	struct interface *linter,
	int traptype,
	int version
	)
{
	size_t n;
	struct ctl_trap *tp;
	struct ctl_trap *tptouse;

	/*
	 * See if we can find this trap.  If so, we only need update
	 * the flags and the time.
	 */
	if ((tp = ctlfindtrap(raddr, linter)) != NULL) {
		switch (traptype) {

		case TRAP_TYPE_CONFIG:
			tp->tr_flags = TRAP_INUSE|TRAP_CONFIGURED;
			break;

		case TRAP_TYPE_PRIO:
			if (tp->tr_flags & TRAP_CONFIGURED)
				return (1); /* don't change anything */
			tp->tr_flags = TRAP_INUSE;
			break;

		case TRAP_TYPE_NONPRIO:
			if (tp->tr_flags & TRAP_CONFIGURED)
				return (1); /* don't change anything */
			tp->tr_flags = TRAP_INUSE|TRAP_NONPRIO;
			break;
		}
		tp->tr_settime = current_time;
		tp->tr_resets++;
		return (1);
	}

	/*
	 * First we heard of this guy.	Try to find a trap structure
	 * for him to use, clearing out lesser priority guys if we
	 * have to. Clear out anyone who's expired while we're at it.
	 */
	tptouse = NULL;
	for (n = 0; n < COUNTOF(ctl_traps); n++) {
		tp = &ctl_traps[n];
		if ((TRAP_INUSE & tp->tr_flags) &&
		    !(TRAP_CONFIGURED & tp->tr_flags) &&
		    ((tp->tr_settime + CTL_TRAPTIME) > current_time)) {
			tp->tr_flags = 0;
			num_ctl_traps--;
		}
		if (!(TRAP_INUSE & tp->tr_flags)) {
			tptouse = tp;
		} else if (!(TRAP_CONFIGURED & tp->tr_flags)) {
			switch (traptype) {

			case TRAP_TYPE_CONFIG:
				if (tptouse == NULL) {
					tptouse = tp;
					break;
				}
				if ((TRAP_NONPRIO & tptouse->tr_flags) &&
				    !(TRAP_NONPRIO & tp->tr_flags))
					break;

				if (!(TRAP_NONPRIO & tptouse->tr_flags)
				    && (TRAP_NONPRIO & tp->tr_flags)) {
					tptouse = tp;
					break;
				}
				if (tptouse->tr_origtime <
				    tp->tr_origtime)
					tptouse = tp;
				break;

			case TRAP_TYPE_PRIO:
				if ( TRAP_NONPRIO & tp->tr_flags) {
					if (tptouse == NULL ||
					    ((TRAP_INUSE &
					      tptouse->tr_flags) &&
					     tptouse->tr_origtime <
					     tp->tr_origtime))
						tptouse = tp;
				}
				break;

			case TRAP_TYPE_NONPRIO:
				break;
			}
		}
	}

	/*
	 * If we don't have room for him return an error.
	 */
	if (tptouse == NULL)
		return (0);

	/*
	 * Set up this structure for him.
	 */
	tptouse->tr_settime = tptouse->tr_origtime = current_time;
	tptouse->tr_count = tptouse->tr_resets = 0;
	tptouse->tr_sequence = 1;
	tptouse->tr_addr = *raddr;
	tptouse->tr_localaddr = linter;
	tptouse->tr_version = (u_char) version;
	tptouse->tr_flags = TRAP_INUSE;
	if (traptype == TRAP_TYPE_CONFIG)
		tptouse->tr_flags |= TRAP_CONFIGURED;
	else if (traptype == TRAP_TYPE_NONPRIO)
		tptouse->tr_flags |= TRAP_NONPRIO;
	num_ctl_traps++;
	return (1);
}