pg_vfprintf(FILE *stream, const char *fmt, va_list args)
{
	PrintfTarget target;
	char		buffer[1024];	/* size is arbitrary */

	if (stream == NULL)
	{
		errno = EINVAL;
		return -1;
	}
	target.bufstart = target.bufptr = buffer;
	target.bufend = buffer + sizeof(buffer) - 1;
	target.stream = stream;
	target.nchars = 0;
	if (dopr(&target, fmt, args))
	{
		errno = EINVAL;			/* bad format */
		return -1;
	}
	/* dump any remaining buffer contents */
	flushbuffer(&target);
	return target.nchars;
}