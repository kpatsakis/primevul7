_g_file_new_home_relative (const char *partial_uri)
{
	GFile *file;
	char  *uri;

	uri = g_strconcat (_g_uri_get_home (), "/", partial_uri, NULL);
	file = g_file_new_for_uri (uri);
	g_free (uri);

	return file;
}