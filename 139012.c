sound_save ()
{
	int fd, i;
	char buf[512];

	fd = hexchat_open_file ("sound.conf", O_CREAT | O_TRUNC | O_WRONLY, 0x180,
								 XOF_DOMODE);
	if (fd == -1)
		return;

	for (i = 0; i < NUM_XP; i++)
	{
		if (sound_files[i] && sound_files[i][0])
		{
			write (fd, buf, g_snprintf (buf, sizeof (buf),
											  "event=%s\n", te[i].name));
			write (fd, buf, g_snprintf (buf, sizeof (buf),
											  "sound=%s\n\n", sound_files[i]));
		}
	}

	close (fd);
}