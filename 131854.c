pg_sprintf(char *str, const char *fmt,...)
{
	int			len;
	va_list		args;

	va_start(args, fmt);
	len = pg_vsprintf(str, fmt, args);
	va_end(args);
	return len;
}