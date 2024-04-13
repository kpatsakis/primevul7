pk_transaction_get_content_type_for_file (const gchar *filename, GError **error)
{
	g_autoptr(GError) error_local = NULL;
	g_autoptr(GFile) file = NULL;
	g_autoptr(GFileInfo) info = NULL;

	/* get file info synchronously */
	file = g_file_new_for_path (filename);
	info = g_file_query_info (file, "standard::content-type",
				  G_FILE_QUERY_INFO_NONE, NULL, &error_local);
	if (info == NULL) {
		g_set_error (error, 1, 0,
			     "failed to get file attributes for %s: %s",
			     filename, error_local->message);
		return NULL;
	}

	/* get content type as string */
	return g_file_info_get_attribute_as_string (info, "standard::content-type");
}