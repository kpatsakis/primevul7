pevent_make_pntevts (void)
{
	int i, m;
	char out[1024];

	for (i = 0; i < NUM_XP; i++)
	{
		g_free (pntevts[i]);
		if (pevt_build_string (pntevts_text[i], &(pntevts[i]), &m) != 0)
		{
			g_snprintf (out, sizeof (out),
						 _("Error parsing event %s.\nLoading default."), te[i].name);
			fe_message (out, FE_MSG_WARN);
			g_free (pntevts_text[i]);
			/* make-te.c sets this 128 flag (DON'T call gettext() flag) */
			if (te[i].num_args & 128)
				pntevts_text[i] = g_strdup (te[i].def);
			else
				pntevts_text[i] = g_strdup (_(te[i].def));
			if (pevt_build_string (pntevts_text[i], &(pntevts[i]), &m) != 0)
			{
				fprintf (stderr,
							"HexChat CRITICAL *** default event text failed to build!\n");
				abort ();
			}
		}
	}
}