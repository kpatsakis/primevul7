_g_str_get_last_field (const char *line,
		       int         last_field)
{
	const char *field;
	int         i;

	if (line == NULL)
		return NULL;

	last_field--;
	field = _g_str_eat_spaces (line);
	for (i = 0; i < last_field; i++) {
		if (field == NULL)
			return NULL;
		field = strchr (field, ' ');
		field = _g_str_eat_spaces (field);
	}

	return field;
}