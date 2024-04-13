percent_dollar_expand(const char *string, ...)
{
	char *ret;
	int err;
	va_list ap;

	va_start(ap, string);
	ret = vdollar_percent_expand(&err, 1, 1, string, ap);
	va_end(ap);
	if (err)
		fatal_f("failed");
	return ret;
}