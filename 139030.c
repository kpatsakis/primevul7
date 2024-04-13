log_open_or_close (session *sess)
{
	if (sess->text_logging == SET_DEFAULT)
	{
		if (prefs.hex_irc_logging)
			log_open (sess);
		else
			log_close (sess);
	}
	else
	{
		if (sess->text_logging)
			log_open (sess);
		else
			log_close (sess);
	}
}