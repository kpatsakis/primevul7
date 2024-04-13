PrintTextTimeStampf (session *sess, time_t timestamp, const char *format, ...)
{
	va_list args;
	char *buf;

	va_start (args, format);
	buf = g_strdup_vprintf (format, args);
	va_end (args);

	PrintTextTimeStamp (sess, buf, timestamp);
	g_free (buf);
}