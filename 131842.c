pg_snprintf(char *str, size_t count, const char *fmt,...)
{
	int			len;
	va_list		args;

	va_start(args, fmt);
	len = pg_vsnprintf(str, count, fmt, args);
	va_end(args);
	return len;
}