tstflags(
	u_long val
	)
{
	register char *cp, *s;
	size_t cb;
	register int i;
	register const char *sep;

	sep = "";
	s = cp = circ_buf[nextcb];
	if (++nextcb >= NUMCB)
		nextcb = 0;
	cb = sizeof(circ_buf[0]);

	snprintf(cp, cb, "%02lx", val);
	cp += strlen(cp);
	cb -= strlen(cp);
	if (!val) {
		strlcat(cp, " ok", cb);
		cp += strlen(cp);
		cb -= strlen(cp);
	} else {
		if (cb) {
			*cp++ = ' ';
			cb--;
		}
		for (i = 0; i < (int)COUNTOF(tstflagnames); i++) {
			if (val & 0x1) {
				snprintf(cp, cb, "%s%s", sep,
					 tstflagnames[i]);
				sep = ", ";
				cp += strlen(cp);
				cb -= strlen(cp);
			}
			val >>= 1;
		}
	}
	if (cb)
		*cp = '\0';

	return s;
}