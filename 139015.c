PrintTextTimeStamp (session *sess, char *text, time_t timestamp)
{
	if (!sess)
	{
		if (!sess_list)
			return;
		sess = (session *) sess_list->data;
	}

	/* make sure it's valid utf8 */
	if (text[0] == '\0')
	{
		text = g_strdup ("\n");
	}
	else
	{
		text = text_fixup_invalid_utf8 (text, -1, NULL);
	}

	log_write (sess, text, timestamp);
	scrollback_save (sess, text, timestamp);
	fe_print_text (sess, text, timestamp, FALSE);
	g_free (text);
}