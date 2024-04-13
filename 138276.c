_g_uri_get_root (const char *uri)
{
	char *host;
	char *root;

	host = _g_uri_get_host (uri);
	if (host == NULL)
		return NULL;
	root = g_strconcat (host, "/", NULL);
	g_free (host);

	return root;
}