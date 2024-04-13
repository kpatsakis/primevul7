sound_play (const char *file, gboolean quiet)
{
	char *buf;
	char *wavfile;
#ifndef WIN32
	char *cmd;
#endif

	/* the pevents GUI editor triggers this after removing a soundfile */
	if (!file[0])
	{
		return;
	}

	/* check for fullpath */
	if (g_path_is_absolute (file))
	{
		wavfile = g_strdup (file);
	}
	else
	{
		wavfile = g_build_filename (get_xdir (), HEXCHAT_SOUND_DIR, file, NULL);
	}

	if (g_access (wavfile, R_OK) == 0)
	{
#ifdef WIN32
		gunichar2 *wavfile_utf16 = g_utf8_to_utf16 (wavfile, -1, NULL, NULL, NULL);

		if (wavfile_utf16 != NULL)
		{
			PlaySoundW (wavfile_utf16, NULL, SND_NODEFAULT | SND_FILENAME | SND_ASYNC);

			g_free (wavfile_utf16);
		}
#else
#ifdef USE_LIBCANBERRA
		if (ca_con == NULL)
		{
			ca_context_create (&ca_con);
			ca_context_change_props (ca_con,
											CA_PROP_APPLICATION_ID, "hexchat",
											CA_PROP_APPLICATION_NAME, "HexChat",
											CA_PROP_APPLICATION_ICON_NAME, "hexchat", NULL);
		}

		if (ca_context_play (ca_con, 0, CA_PROP_MEDIA_FILENAME, wavfile, NULL) != 0)
#endif
		{
			cmd = g_find_program_in_path ("play");
	
			if (cmd)
			{
				buf = g_strdup_printf ("%s \"%s\"", cmd, wavfile);
				hexchat_exec (buf);
				g_free (buf);
				g_free (cmd);
			}
		}
#endif
	}
	else
	{
		if (!quiet)
		{
			buf = g_strdup_printf (_("Cannot read sound file:\n%s"), wavfile);
			fe_message (buf, FE_MSG_ERROR);
			g_free (buf);
		}
	}

	g_free (wavfile);
}