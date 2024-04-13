_g_file_get_home (void)
{
	static GFile *file = NULL;

	if (file != NULL)
		return file;

	file = g_file_new_for_path (g_get_home_dir ());

	return file;
}