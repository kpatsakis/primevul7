sound_load_event (char *evt, char *file)
{
	int i = 0;

	if (file[0] && pevent_find (evt, &i) != -1)
	{
		g_free (sound_files[i]);
		sound_files[i] = g_strdup (file);
	}
}