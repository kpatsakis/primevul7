_g_uri_get_home (void)
{
	static char *home_uri = NULL;
	if (home_uri == NULL)
		home_uri = g_filename_to_uri (g_get_home_dir (), NULL, NULL);
	return home_uri;
}