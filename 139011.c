text_find_format_string (char *name)
{
	int i = 0;

	i = pevent_find (name, &i);
	if (i >= 0)
		return pntevts_text[i];

	return NULL;
}