_g_file_append_path (GFile  *file,
		     ...)
{
	char       *uri;
	const char *path;
	va_list     args;
	GFile      *new_file;

	uri = g_file_get_uri (file);

	va_start (args, file);
	while ((path = va_arg (args, const char *)) != NULL) {
		char *escaped;
		char *new_uri;

		escaped = g_uri_escape_string (path, G_URI_RESERVED_CHARS_ALLOWED_IN_PATH, FALSE);
		new_uri = g_build_filename (uri, escaped, NULL);
		g_free (uri);
		uri = new_uri;

		g_free (escaped);
	}
	va_end (args);
	new_file = g_file_new_for_uri (uri);

	g_free (uri);

	return new_file;
}