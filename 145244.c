outputarr(
	FILE *fp,
	char *name,
	int narr,
	l_fp *lfp
	)
{
	register char *bp;
	register char *cp;
	register int i;
	register int len;
	char buf[256];

	bp = buf;
	/*
	 * Hack to align delay and offset values
	 */
	for (i = (int)strlen(name); i < 11; i++)
	    *bp++ = ' ';

	for (i = narr; i > 0; i--) {
		if (i != narr)
		    *bp++ = ' ';
		cp = lfptoms(lfp, 2);
		len = strlen(cp);
		if (len > 7) {
			cp[7] = '\0';
			len = 7;
		}
		while (len < 7) {
			*bp++ = ' ';
			len++;
		}
		while (*cp != '\0')
		    *bp++ = *cp++;
		lfp++;
	}
	*bp = '\0';
	output(fp, name, buf);
}