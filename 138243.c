_g_path_remove_first_extension (const gchar *path)
{
	const char *ext;

	if (path == NULL)
		return NULL;

	ext = strrchr (path, '.');
	if (ext == NULL)
		return g_strdup (path);
	else
		return g_strndup (path, strlen (path) - strlen (ext));
}