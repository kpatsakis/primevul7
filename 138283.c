_g_path_get_relative_basename_safe (const char *path,
				    const char *base_dir,
				    gboolean    junk_paths)
{
	return sanitize_filename (_g_path_get_relative_basename (path, base_dir, junk_paths));
}