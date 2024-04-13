pg_vsprintf(char *str, const char *fmt, va_list args)
{
	PrintfTarget target;

	if (str == NULL)
		return 0;
	target.bufstart = target.bufptr = str;
	target.bufend = NULL;
	target.stream = NULL;
	/* target.nchars is unused in this case */
	if (dopr(&target, fmt, args))
	{
		*(target.bufptr) = '\0';
		errno = EINVAL;			/* bad format */
		return -1;
	}
	*(target.bufptr) = '\0';
	return target.bufptr - target.bufstart;
}