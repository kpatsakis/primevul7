convtime(const char *s)
{
	long total, secs, multiplier;
	const char *p;
	char *endp;

	errno = 0;
	total = 0;
	p = s;

	if (p == NULL || *p == '\0')
		return -1;

	while (*p) {
		secs = strtol(p, &endp, 10);
		if (p == endp ||
		    (errno == ERANGE && (secs == INT_MIN || secs == INT_MAX)) ||
		    secs < 0)
			return -1;

		multiplier = 1;
		switch (*endp++) {
		case '\0':
			endp--;
			break;
		case 's':
		case 'S':
			break;
		case 'm':
		case 'M':
			multiplier = MINUTES;
			break;
		case 'h':
		case 'H':
			multiplier = HOURS;
			break;
		case 'd':
		case 'D':
			multiplier = DAYS;
			break;
		case 'w':
		case 'W':
			multiplier = WEEKS;
			break;
		default:
			return -1;
		}
		if (secs > INT_MAX / multiplier)
			return -1;
		secs *= multiplier;
		if  (total > INT_MAX - secs)
			return -1;
		total += secs;
		if (total < 0)
			return -1;
		p = endp;
	}

	return total;
}