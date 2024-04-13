pg_fprintf(FILE *stream, const char *fmt,...)
{
	int			len;
	va_list		args;

	va_start(args, fmt);
	len = pg_vfprintf(stream, fmt, args);
	va_end(args);
	return len;
}