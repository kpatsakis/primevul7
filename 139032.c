pevent_load_defaults (void)
{
	int i;

	for (i = 0; i < NUM_XP; i++)
	{
		g_free (pntevts_text[i]);

		/* make-te.c sets this 128 flag (DON'T call gettext() flag) */
		if (te[i].num_args & 128)
			pntevts_text[i] = g_strdup (te[i].def);
		else
			pntevts_text[i] = g_strdup (_(te[i].def));
	}
}