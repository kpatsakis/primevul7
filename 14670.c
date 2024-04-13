dollar_expand(int *parseerr, const char *string, ...)
{
	char *ret;
	int err;
	va_list ap;

	va_start(ap, string);
	ret = vdollar_percent_expand(&err, 1, 0, string, ap);
	va_end(ap);
	if (parseerr != NULL)
		*parseerr = err;
	return ret;
}