log_create_filename (char *channame)
{
	char *tmp, *ret;
	int mbl;

	ret = tmp = g_strdup (channame);
	while (*tmp)
	{
		mbl = g_utf8_skip[((unsigned char *)tmp)[0]];
		if (mbl == 1)
		{
#ifndef WIN32
			*tmp = rfc_tolower (*tmp);
			if (*tmp == '/')
#else
			/* win32 can't handle filenames with \|/><:"*? characters */
			if (*tmp == '\\' || *tmp == '|' || *tmp == '/' ||
				 *tmp == '>'  || *tmp == '<' || *tmp == ':' ||
				 *tmp == '\"' || *tmp == '*' || *tmp == '?')
#endif
				*tmp = '_';
		}
		tmp += mbl;
	}

	return ret;
}