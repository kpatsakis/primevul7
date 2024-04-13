_g_file_is_local (GFile *file)
{
	char     *scheme;
	gboolean  is_local;

	scheme = g_file_get_uri_scheme (file);
	is_local = strcmp (scheme, "file") == 0;

	g_free (scheme);

	return is_local;
}