_g_file_get_display_basename (GFile *file)
{
	char *uri, *e_name, *name;

	uri = g_file_get_uri (file);
	e_name = g_filename_display_basename (uri);
	name = g_uri_unescape_string (e_name, "");

	g_free (e_name);
	g_free (uri);

	return name;
}