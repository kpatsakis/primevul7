_g_filename_get_extension (const char *filename)
{
	const char *ptr = filename;
	int         len;
	int         p;
	const char *ext;

	if (filename == NULL)
		return NULL;

	len = strlen (filename);
	if (len <= 1)
		return NULL;

	p = len - 1;
	while ((p >= 0) && (ptr[p] != '.'))
		p--;
	if (p < 0)
		return NULL;

	ext = filename + p;
	if (ext - 4 > filename) {
		const char *test = ext - 4;
		/* .tar.rz cannot be uncompressed in one step */
		if ((strncmp (test, ".tar", 4) == 0) && (strncmp (ext, ".rz", 2) != 0))
			ext = ext - 4;
	}
	return ext;
}