vwarning(const char *fmt, va_list ap)
{
	int serrno = errno;
	(void) fprintf(stderr, "%s: ", progname);
	vfprintf(stderr, fmt, ap);
	(void) fprintf(stderr, ": %s", strerror(serrno));
}