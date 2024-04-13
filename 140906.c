ctl_printf(
	const char * fmt,
	...
	)
{
	static const char * ellipsis = "[...]";
	va_list va;
	char    fmtbuf[128];
	int     rc;
	
	va_start(va, fmt);
	rc = vsnprintf(fmtbuf, sizeof(fmtbuf), fmt, va);
	va_end(va);
	if (rc < 0 || rc >= sizeof(fmtbuf))
		strcpy(fmtbuf + sizeof(fmtbuf) - strlen(ellipsis) - 1,
		       ellipsis);
	ctl_putdata(fmtbuf, strlen(fmtbuf), 0);
}