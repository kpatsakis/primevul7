cleanup_pathname_win(char *path)
{
	wchar_t wc;
	char *p;
	size_t alen, l;
	int mb, complete, utf8;

	alen = 0;
	mb = 0;
	complete = 1;
	utf8 = (strcmp(nl_langinfo(CODESET), "UTF-8") == 0)? 1: 0;
	for (p = path; *p != '\0'; p++) {
		++alen;
		if (*p == '\\') {
			/* If previous byte is smaller than 128,
			 * this is not second byte of multibyte characters,
			 * so we can replace '\' with '/'. */
			if (utf8 || !mb)
				*p = '/';
			else
				complete = 0;/* uncompleted. */
		} else if (*(unsigned char *)p > 127)
			mb = 1;
		else
			mb = 0;
		/* Rewrite the path name if its next character is unusable. */
		if (*p == ':' || *p == '*' || *p == '?' || *p == '"' ||
		    *p == '<' || *p == '>' || *p == '|')
			*p = '_';
	}
	if (complete)
		return;

	/*
	 * Convert path separator in wide-character.
	 */
	p = path;
	while (*p != '\0' && alen) {
		l = mbtowc(&wc, p, alen);
		if (l == (size_t)-1) {
			while (*p != '\0') {
				if (*p == '\\')
					*p = '/';
				++p;
			}
			break;
		}
		if (l == 1 && wc == L'\\')
			*p = '/';
		p += l;
		alen -= l;
	}
}