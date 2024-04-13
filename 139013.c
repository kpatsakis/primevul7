mkdir_p (char *filename)
{
	char *dirname, *dirname_fs;
	GError *err = NULL;
	
	dirname = g_path_get_dirname (filename);
	dirname_fs = g_filename_from_utf8 (dirname, -1, NULL, NULL, &err);
	if (!dirname_fs)
	{
		g_warning ("%s", err->message);
		g_error_free (err);
		g_free (dirname);
		return;
	}

	g_mkdir_with_parents (dirname_fs, 0700);

	g_free (dirname);
	g_free (dirname_fs);
}