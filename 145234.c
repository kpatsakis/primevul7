decodearr(
	char *str,
	int *narr,
	l_fp *lfparr
	)
{
	register char *cp, *bp;
	register l_fp *lfp;
	char buf[60];

	lfp = lfparr;
	cp = str;
	*narr = 0;

	while (*narr < 8) {
		while (isspace((int)*cp))
		    cp++;
		if (*cp == '\0')
		    break;

		bp = buf;
		while (!isspace((int)*cp) && *cp != '\0')
		    *bp++ = *cp++;
		*bp++ = '\0';

		if (!decodetime(buf, lfp))
		    return 0;
		(*narr)++;
		lfp++;
	}
	return 1;
}