_g_file_cmp_uris (GFile *a,
                  GFile *b)
{
	char *uri_a;
	char *uri_b;
	int   result;

	uri_a = g_file_get_uri (a);
	uri_b = g_file_get_uri (b);
	result = g_strcmp0 (uri_a, uri_b);

	g_free (uri_b);
	g_free (uri_a);

	return result;
}