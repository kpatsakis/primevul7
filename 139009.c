pevent_trigger_load (int *i_penum, char **i_text, char **i_snd)
{
	int penum = *i_penum;
	char *text = *i_text, *snd = *i_snd;

	if (penum != -1 && text != NULL)
	{
		g_free (pntevts_text[penum]);
		pntevts_text[penum] = g_strdup (text);
	}

	g_free (text);
	g_free (snd);
	*i_text = NULL;
	*i_snd = NULL;
	*i_penum = 0;
}