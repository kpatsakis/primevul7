text_emit_by_name (char *name, session *sess, time_t timestamp,
				   char *a, char *b, char *c, char *d)
{
	int i = 0;

	i = pevent_find (name, &i);
	if (i >= 0)
	{
		text_emit (i, sess, a, b, c, d, timestamp);
		return 1;
	}

	return 0;
}