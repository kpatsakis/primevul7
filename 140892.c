ctl_getitem(
	const struct ctl_var *var_list,
	char **data
	)
{
	static const struct ctl_var eol = { 0, EOV, NULL };
	static char buf[128];
	static u_long quiet_until;
	const struct ctl_var *v;
	const char *pch;
	char *cp;
	char *tp;

	/*
	 * Delete leading commas and white space
	 */
	while (reqpt < reqend && (*reqpt == ',' ||
				  isspace((unsigned char)*reqpt)))
		reqpt++;
	if (reqpt >= reqend)
		return NULL;

	if (NULL == var_list)
		return &eol;

	/*
	 * Look for a first character match on the tag.  If we find
	 * one, see if it is a full match.
	 */
	cp = reqpt;
	for (v = var_list; !(EOV & v->flags); v++) {
		if (!(PADDING & v->flags) && *cp == *(v->text)) {
			pch = v->text;
			while ('\0' != *pch && '=' != *pch && cp < reqend
			       && *cp == *pch) {
				cp++;
				pch++;
			}
			if ('\0' == *pch || '=' == *pch) {
				while (cp < reqend && isspace((u_char)*cp))
					cp++;
				if (cp == reqend || ',' == *cp) {
					buf[0] = '\0';
					*data = buf;
					if (cp < reqend)
						cp++;
					reqpt = cp;
					return v;
				}
				if ('=' == *cp) {
					cp++;
					tp = buf;
					while (cp < reqend && isspace((u_char)*cp))
						cp++;
					while (cp < reqend && *cp != ',') {
						*tp++ = *cp++;
						if ((size_t)(tp - buf) >= sizeof(buf)) {
							ctl_error(CERR_BADFMT);
							numctlbadpkts++;
							NLOG(NLOG_SYSEVENT)
								if (quiet_until <= current_time) {
									quiet_until = current_time + 300;
									msyslog(LOG_WARNING,
"Possible 'ntpdx' exploit from %s#%u (possibly spoofed)", stoa(rmt_addr), SRCPORT(rmt_addr));
								}
							return NULL;
						}
					}
					if (cp < reqend)
						cp++;
					*tp-- = '\0';
					while (tp >= buf && isspace((u_char)*tp))
						*tp-- = '\0';
					reqpt = cp;
					*data = buf;
					return v;
				}
			}
			cp = reqpt;
		}
	}
	return v;
}