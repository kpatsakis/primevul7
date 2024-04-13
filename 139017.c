sound_load ()
{
	int fd;
	char buf[512];
	char evt[128];

	memset (&sound_files, 0, sizeof (char *) * (NUM_XP));

	fd = hexchat_open_file ("sound.conf", O_RDONLY, 0, 0);
	if (fd == -1)
		return;

	evt[0] = 0;
	while (waitline (fd, buf, sizeof buf, FALSE) != -1)
	{
		if (strncmp (buf, "event=", 6) == 0)
		{
			safe_strcpy (evt, buf + 6, sizeof (evt));
		}
		else if (strncmp (buf, "sound=", 6) == 0)
		{
			if (evt[0] != 0)
			{
				sound_load_event (evt, buf + 6);
				evt[0] = 0;
			}
		}
	}

	close (fd);
}