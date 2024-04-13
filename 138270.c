_g_path_get_relative_basename (const char *path,
			       const char *base_dir,
			       gboolean    junk_paths)
{
	int         base_dir_len;
	const char *base_path;

	if (junk_paths)
		return _g_path_get_basename (path);

	if (base_dir == NULL)
		return (path[0] == '/') ? path + 1 : path;

	base_dir_len = strlen (base_dir);
	if (strlen (path) < base_dir_len)
		return NULL;

	base_path = path + base_dir_len;
	if (path[0] != '/')
		base_path -= 1;

	return base_path;
}