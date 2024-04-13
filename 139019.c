pevent_save (char *fn)
{
	int fd, i;
	char buf[1024];

	if (!fn)
		fd = hexchat_open_file ("pevents.conf", O_CREAT | O_TRUNC | O_WRONLY,
									 0x180, XOF_DOMODE);
	else
		fd = hexchat_open_file (fn, O_CREAT | O_TRUNC | O_WRONLY, 0x180,
									 XOF_FULLPATH | XOF_DOMODE);
	if (fd == -1)
	{
		/*
		   fe_message ("Error opening config file\n", FALSE); 
		   If we get here when X-Chat is closing the fe-message causes a nice & hard crash
		   so we have to use perror which doesn't rely on GTK
		 */

		perror ("Error opening config file\n");
		return;
	}

	for (i = 0; i < NUM_XP; i++)
	{
		write (fd, buf, g_snprintf (buf, sizeof (buf),
										  "event_name=%s\n", te[i].name));
		write (fd, buf, g_snprintf (buf, sizeof (buf),
										  "event_text=%s\n\n", pntevts_text[i]));
	}

	close (fd);
}