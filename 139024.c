scrollback_load (session *sess)
{
	GInputStream *stream;
	GDataInputStream *istream;
	gchar *buf, *text;
	gint lines = 0;
	time_t stamp = 0;

	if (sess->text_scrollback == SET_DEFAULT)
	{
		if (!prefs.hex_text_replay)
			return;
	}
	else
	{
		if (sess->text_scrollback != SET_ON)
			return;
	}

	if (!sess->scrollfile)
	{
		if ((buf = scrollback_get_filename (sess)) == NULL)
			return;

		sess->scrollfile = g_file_new_for_path (buf);
		g_free (buf);
	}

	stream = G_INPUT_STREAM(g_file_read (sess->scrollfile, NULL, NULL));
	if (!stream)
		return;

	istream = g_data_input_stream_new (stream);
	/*
	 * This is to avoid any issues moving between windows/unix
	 * but the docs mention an invalid \r without a following \n
	 * can lock up the program... (Our write() always adds \n)
	 */
	g_data_input_stream_set_newline_type (istream, G_DATA_STREAM_NEWLINE_TYPE_ANY);
	g_object_unref (stream);

	while (1)
	{
		GError *err = NULL;
		gsize n_bytes;

		buf = g_data_input_stream_read_line_utf8 (istream, &n_bytes, NULL, &err);

		if (!err && buf)
		{
			/*
			 * Some scrollback lines have three blanks after the timestamp and a newline
			 * Some have only one blank and a newline
			 * Some don't even have a timestamp
			 * Some don't have any text at all
			 */
			if (buf[0] == 'T' && buf[1] == ' ')
			{
				if (sizeof (time_t) == 4)
					stamp = strtoul (buf + 2, NULL, 10);
				else
					stamp = g_ascii_strtoull (buf + 2, NULL, 10); /* in case time_t is 64 bits */

				if (G_UNLIKELY(stamp == 0))
				{
					g_warning ("Invalid timestamp in scrollback file");
					continue;
				}

				text = strchr (buf + 3, ' ');
				if (text && text[1])
				{
					if (prefs.hex_text_stripcolor_replay)
					{
						text = strip_color (text + 1, -1, STRIP_COLOR);
					}

					fe_print_text (sess, text, stamp, TRUE);

					if (prefs.hex_text_stripcolor_replay)
					{
						g_free (text);
					}
				}
				else
				{
					fe_print_text (sess, "  ", stamp, TRUE);
				}
			}
			else
			{
				if (strlen (buf))
					fe_print_text (sess, buf, 0, TRUE);
				else
					fe_print_text (sess, "  ", 0, TRUE);
			}
			lines++;

			g_free (buf);
		}
		else if (err)
		{
			/* If its only an encoding error it may be specific to the line */
			if (g_error_matches (err, G_CONVERT_ERROR, G_CONVERT_ERROR_ILLEGAL_SEQUENCE))
			{
				g_warning ("Invalid utf8 in scrollback file");
				g_clear_error (&err);
				continue;
			}

			/* For general errors just give up */
			g_clear_error (&err);
			break;
		}
		else /* No new line */
		{
			break;
		}
	}

	g_object_unref (istream);

	sess->scrollwritten = lines;

	if (lines)
	{
		text = ctime (&stamp);
		buf = g_strdup_printf ("\n*\t%s %s\n", _("Loaded log from"), text);
		fe_print_text (sess, buf, 0, TRUE);
		g_free (buf);
		/*EMIT_SIGNAL (XP_TE_GENMSG, sess, "*", buf, NULL, NULL, NULL, 0);*/
	}
}