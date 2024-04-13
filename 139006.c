log_write (session *sess, char *text, time_t ts)
{
	char *temp;
	char *stamp;
	char *file;
	int len;

	if (sess->text_logging == SET_DEFAULT)
	{
		if (!prefs.hex_irc_logging)
			return;
	}
	else
	{
		if (sess->text_logging != SET_ON)
			return;
	}

	if (sess->logfd == -1)
	{
		log_open (sess);
	}

	/* change to a different log file? */
	file = log_create_pathname (sess->server->servername, sess->channel, server_get_network (sess->server, FALSE));
	if (file)
	{
		if (g_access (file, F_OK) != 0)
		{
			if (sess->logfd != -1)
			{
				close (sess->logfd);
			}

			sess->logfd = log_open_file (sess->server->servername, sess->channel, server_get_network (sess->server, FALSE));
		}

		g_free (file);
	}

	if (sess->logfd == -1)
	{
		return;
	}

	if (prefs.hex_stamp_log)
	{
		if (!ts) ts = time(0);
		len = get_stamp_str (prefs.hex_stamp_log_format, ts, &stamp);
		if (len)
		{
			write (sess->logfd, stamp, len);
			g_free (stamp);
		}
	}

	temp = strip_color (text, -1, STRIP_ALL);
	len = strlen (temp);
	write (sess->logfd, temp, len);
	/* lots of scripts/plugins print without a \n at the end */
	if (temp[len - 1] != '\n')
		write (sess->logfd, "\n", 1);	/* emulate what xtext would display */
	g_free (temp);
}