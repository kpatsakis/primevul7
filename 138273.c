_g_time_to_string (time_t time)
{
	struct tm *tm;
	char       s_time[256];
	char      *locale_format = NULL;
	char      *time_utf8;

	tm = localtime (&time);
	/* This is the time format used in the "Date Modified" column and
	 * in the Properties dialog.  See the man page of strftime for an
	 * explanation of the values. */
	locale_format = g_locale_from_utf8 (_("%d %B %Y, %H:%M"), -1, NULL, NULL, NULL);
	strftime (s_time, sizeof (s_time) - 1, locale_format, tm);
	g_free (locale_format);
	time_utf8 = g_locale_to_utf8 (s_time, -1, NULL, NULL, NULL);

	return time_utf8;
}