log_open (session *sess)
{
	static gboolean log_error = FALSE;

	log_close (sess);
	sess->logfd = log_open_file (sess->server->servername, sess->channel,
										  server_get_network (sess->server, FALSE));

	if (!log_error && sess->logfd == -1)
	{
		char *filename = log_create_pathname (sess->server->servername, sess->channel, server_get_network (sess->server, FALSE));
		char *message = g_strdup_printf (_("* Can't open log file(s) for writing. Check the\npermissions on %s"), filename);

		g_free (filename);

		fe_message (message, FE_MSG_WAIT | FE_MSG_ERROR);

		g_free (message);

		log_error = TRUE;
	}
}