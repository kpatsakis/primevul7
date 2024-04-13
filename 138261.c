g_utf8_strchomp (char *string)
{
	char   *scan;
	gsize   len;

	g_return_val_if_fail (string != NULL, NULL);

	len = g_utf8_strlen (string, -1);

	if (len == 0)
		return string;

	scan = g_utf8_offset_to_pointer (string, len - 1);

	while (len--) {
		gunichar c = g_utf8_get_char (scan);
		if (g_unichar_isspace (c))
			*scan = '\0';
		else
			break;
		scan = g_utf8_find_prev_char (string, scan);
	}

	return string;
}