count_chars_to_escape (const char *str,
		       const char *meta_chars)
{
	int         meta_chars_n = strlen (meta_chars);
	const char *s;
	int         n = 0;

	for (s = str; *s != 0; s++) {
		int i;
		for (i = 0; i < meta_chars_n; i++)
			if (*s == meta_chars[i]) {
				n++;
				break;
			}
	}

	return n;
}