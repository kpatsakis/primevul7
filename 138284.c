_g_str_eat_void_chars (const char *line)
{
	if (line == NULL)
		return NULL;
	while (((*line == ' ') || (*line == '\t')) && (*line != 0))
		line++;
	return line;
}