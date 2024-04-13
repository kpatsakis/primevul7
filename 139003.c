scrollback_shrink (session *sess)
{
	char *buf, *p;
	gsize len;
	gint offset, lines = 0;
	const gint max_lines = MIN(prefs.hex_text_max_lines, SCROLLBACK_MAX);

	if (!g_file_load_contents (sess->scrollfile, NULL, &buf, &len, NULL, NULL))
		return;

	/* count all lines */
	p = buf;
	while (p != buf + len)
	{
		if (*p == '\n')
			lines++;
		p++;
	}

	offset = lines - max_lines;

	/* now just go back to where we want to start the file */
	p = buf;
	lines = 0;
	while (p != buf + len)
	{
		if (*p == '\n')
		{
			lines++;
			if (lines == offset)
			{
				p++;
				break;
			}
		}
		p++;
	}

	if (g_file_replace_contents (sess->scrollfile, p, strlen(p), NULL, FALSE,
							G_FILE_CREATE_PRIVATE, NULL, NULL, NULL))
		sess->scrollwritten = lines;

	g_free (buf);
}