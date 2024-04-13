get_stamp_str (char *fmt, time_t tim, char **ret)
{
	char dest[128];
	gsize len_locale;
	gsize len_utf8;

	/* strftime requires the format string to be in locale encoding. */
	fmt = g_locale_from_utf8 (fmt, -1, NULL, NULL, NULL);

	len_locale = strftime_validated (dest, sizeof (dest), fmt, localtime (&tim));

	g_free (fmt);

	if (len_locale == 0)
	{
		return 0;
	}

	*ret = g_locale_to_utf8 (dest, len_locale, NULL, &len_utf8, NULL);
	if (*ret == NULL)
	{
		return 0;
	}

	return len_utf8;
}