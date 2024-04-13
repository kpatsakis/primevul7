xextendf(char **sp, const char *sep, const char *fmt, ...)
{
	va_list ap;
	char *tmp1, *tmp2;

	va_start(ap, fmt);
	xvasprintf(&tmp1, fmt, ap);
	va_end(ap);

	if (*sp == NULL || **sp == '\0') {
		free(*sp);
		*sp = tmp1;
		return;
	}
	xasprintf(&tmp2, "%s%s%s", *sp, sep == NULL ? "" : sep, tmp1);
	free(tmp1);
	free(*sp);
	*sp = tmp2;
}